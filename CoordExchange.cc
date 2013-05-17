
// vi: tabstop=4:expandtab

#include "CoordExchange.hh"
#include "ProcElement.hh"

#include "fe/FiniteElements.hh"


typedef linalg::CoordMatrixEntry<int64, double> Entry;
typedef linalg::CoordMatrix<int64, double> Matrix;


void GatherRowsAtOwners(const Matrix& send, Matrix& recv)
{
    fe::interface::Mesh* mesh = gcpe()->GetFESolver()->GetMesh();

    /// Since the processing element owns all its mesh nodes
    /// there is nothing to do
    if(1 == gcpe()->myscale.size())
        return;

    /// This is a very dumb algorithm. We send much more than necessary
    /// but that's okay for now ...
    int BW = send.maxBandwidth();
    gcpe()->myscale.allreduce(MPI_IN_PLACE, &BW, 1, MPI_INT, MPI_MAX);

    int N, t, i;
    fe::interface::NodalVector<int> idx(mesh);

    N = mesh->nnodes();
    gcpe()->myscale.exscan(&N, &t, 1, MPI_INT, MPI_SUM);
    if(0 == gcpe()->myscale.myrank())
        t = 0;

    CHECK_DEBUG(t >= 0, 0);

    /// Compute globally unique node identifiers
    i = 0;
    for(fe::interface::Node n = mesh->NodesBegin(); n != mesh->NodesEnd(); ++n)
        idx(n) = t + (i++);
    idx.ParallelCopy(*gcpe()->GetFESolver(),           
              gcpe()->GetFESolver()->GetNodeDistribMap());

#if 1 && !defined(NDEBUG)
    i = 0;
    for(fe::interface::Node n = mesh->NodesBegin(); n != mesh->NodesEnd(); ++n, ++i)
        if(gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(n))
            CHECK_DEBUG(idx(n) == t+i, 0);
#endif

    /// Count the number of rows to send
    N = 0;
    for(fe::interface::Node n = mesh->NodesBegin(); n != mesh->NodesEnd(); ++n)
        if(not gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(n))
            N += 1;

    gcpe()->myscale.allreduce(MPI_IN_PLACE, &N, 1, MPI_INT, MPI_MAX);

    Entry* buf = new Entry[N*BW*gcpe()->myscale.size()];
    for(i = 0; i < N*BW*gcpe()->myscale.size(); ++i)
    {
        buf[i].row = -1;
        buf[i].col = -1;
        buf[i].val = 0.0;
    }

    i = N*BW*gcpe()->myscale.myrank();
    for(fe::interface::Node n = mesh->NodesBegin(); n != mesh->NodesEnd(); ++n)
        if(not gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(n))
        {
            long k = fe::interface::Node::Index(*gcpe()->GetFESolver(), n);
            for(Matrix::ConstIterator it = send.begin(); it != send.end(); ++it)
                if(it->row == k)
                {
                    CHECK_DEBUG(i < N*BW*(gcpe()->myscale.myrank()+1), 0);

                    buf[i].row = idx(n);
                    /// TODO Column indices are not translated. This is fine for
                    ///      the call from TreeAssembler where columns correspond
                    ///      to particles but not if we want to use the function
                    ///      for mass matrices
                    buf[i].col = it->col;
                    buf[i].val = it->val;

                    i += 1;
                }
        }

    gcpe()->myscale.allgather(MPI_IN_PLACE, N*BW, Entry::type(), buf, N*BW, Entry::type());

    for(i = 0; i < gcpe()->myscale.size()*N*BW; ++i)
    {
        if(-1  == buf[i].row)
            continue;
        CHECK_DEBUG(buf[i].row >= 0, 0);

        if(0.0 == buf[i].val)
            continue;

        ///TODO We could get around the quadratic complexity here if we switch the
        ///     outer loops because then we do the interior loop only for the boundary
        ///     nodes
        for(fe::interface::Node n = mesh->NodesBegin(); n != mesh->NodesEnd(); ++n)
        {
            if(not gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(n))
                continue;
        
            if(buf[i].row == idx(n))
            {
                recv.insert(Entry(fe::interface::Node::Index(*gcpe()->GetFESolver(), n), buf[i].col, buf[i].val));
                break;
            }
        }
    }

    delete[] buf;
}

