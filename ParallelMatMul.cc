
// vi: tabstop=4:expandtab

#include "ParallelMatMul.hh"
#include "Logging.hh"
#include "ProcElement.hh"
#include "mpi/Cartcommunicator.hh"


/// DistributeMatrixByRow: Distribute the matrix A so that rows with row index equal to (p | ...)
///                        are stored on processing element p
static void DistributeMatrixByRow(mpi::Communicator& comm, linalg::CoordMatrix<int64, double>& A, 
                                  linalg::CoordMatrix<int64, double>* DA)
{
    int p, *sendcounts, *senddispls, *recvcounts, *recvdispls;
    std::size_t i;
    linalg::CoordMatrix<int64, double>::Entry *sendbuf;

    sendcounts = new int[comm.size()];
    senddispls = new int[comm.size()+1];
    recvcounts = new int[comm.size()];
    recvdispls = new int[comm.size()+1];

    std::fill(sendcounts, sendcounts+comm.size(), 0);
    for(i = 0; i < A.size(); ++i)
    {
        CHECK_DEBUG(Int64::hi(A[i].col) >= 0 and Int64::hi(A[i].col) < comm.size(), 0);
        sendcounts[Int64::hi(A[i].row)] += 1;
    }

    comm.alltoall(sendcounts, 1, MPI_INT, recvcounts, 1, MPI_INT);

    senddispls[0] = 0;
    for(p = 1; p <= comm.size(); ++p)
        senddispls[p] = senddispls[p-1] + sendcounts[p-1];

    recvdispls[0] = 0;
    for(p = 1; p <= comm.size(); ++p)
        recvdispls[p] = recvdispls[p-1] + recvcounts[p-1];

    /// Allocate the buffers
    sendbuf = new linalg::CoordMatrix<int64, double>::Entry[senddispls[comm.size()]];
    Log::Instance().write("Sending %5d entries in %s.\n", senddispls[comm.size()], __FUNCTION__);

    for(i = 0; i < A.size(); ++i)
        sendbuf[senddispls[Int64::hi(A[i].row)]++] = A[i];

    /// We destroyed the senddispls and need to recreate it
    senddispls[0] = 0;
    for(p = 1; p <= comm.size(); ++p)
        senddispls[p] = senddispls[p-1] + sendcounts[p-1];

    DA->resize(recvdispls[comm.size()]);
    comm.alltoallv(sendbuf  , sendcounts, senddispls, A.type(),
                   &(*DA)[0], recvcounts, recvdispls, DA->type());

    delete[] sendcounts;
    delete[] senddispls;
    delete[] recvcounts;
    delete[] recvdispls;
    delete[] sendbuf;    
}

/// DistributeMatrixByCol: Distribute the matrix A so that columns with column index equal to (p | ...)
///                        are stored on processing element p
static void DistributeMatrixByCol(mpi::Communicator& comm, linalg::CoordMatrix<int64, double>& A, 
                                  linalg::CoordMatrix<int64, double>* DA)
{
    int p, *sendcounts, *senddispls, *recvcounts, *recvdispls;
    std::size_t i;
    linalg::CoordMatrix<int64, double>::Entry *sendbuf;

    sendcounts = new int[comm.size()];
    senddispls = new int[comm.size()+1];
    recvcounts = new int[comm.size()];
    recvdispls = new int[comm.size()+1];

    std::fill(sendcounts, sendcounts+comm.size(), 0);
    for(i = 0; i < A.size(); ++i)
    {
        CHECK_DEBUG(Int64::hi(A[i].col) >= 0 and Int64::hi(A[i].col) < comm.size(), 0);
        sendcounts[Int64::hi(A[i].col)] += 1;
    }
    
    comm.alltoall(sendcounts, 1, MPI_INT, recvcounts, 1, MPI_INT);

    senddispls[0] = 0;
    for(p = 1; p <= comm.size(); ++p)
        senddispls[p] = senddispls[p-1] + sendcounts[p-1];

    recvdispls[0] = 0;
    for(p = 1; p <= comm.size(); ++p)
        recvdispls[p] = recvdispls[p-1] + recvcounts[p-1];

    /// Allocate the buffers
    sendbuf = new linalg::CoordMatrix<int64, double>::Entry[senddispls[comm.size()]];
    Log::Instance().write("Sending %5d entries in %s.\n", senddispls[comm.size()], __FUNCTION__);

    if(0 == senddispls[comm.size()])
        CHECK_ALWAYS(0 == A.size(), 0);

    for(i = 0; i < A.size(); ++i)
        sendbuf[senddispls[Int64::hi(A[i].col)]++] = A[i];

    /// We destroyed the senddispls and need to recreate it
    senddispls[0] = 0;
    for(p = 1; p <= comm.size(); ++p)
        senddispls[p] = senddispls[p-1] + sendcounts[p-1];

    DA->resize(recvdispls[comm.size()]);
    comm.alltoallv(sendbuf  , sendcounts, senddispls, A.type(),
                   &(*DA)[0], recvcounts, recvdispls, DA->type());

    delete[] sendcounts;
    delete[] senddispls;
    delete[] recvcounts;
    delete[] recvdispls;
    delete[] sendbuf;
}

void ParallelMatMul(double alpha,
                    linalg::CoordMatrix<int64, double>& A,
                    linalg::CoordMatrix<int64, double>& B,
                    double beta,
                    linalg::CoordMatrix<int64, double>* C)
{
    int sendcount, recvcount, count, left, right, ringSize, ringPer, round;
    mpi::Communicator sub;
    mpi::Cartcommunicator ring;
    mpi::Irequest req;
    /// The redistributed versions of A and B are called 
    /// DA and DB respectively
    linalg::CoordMatrix<int64, double> DA, DB;

    gcpe()->world.barrier();
    /// --------------------------------------------------------------------------------
    /// Step 1: Distribute the matrix A by row and B by column
    DistributeMatrixByRow(gcpe()->world, A, &DA);
    DistributeMatrixByCol(gcpe()->world, B, &DB);
    /// --------------------------------------------------------------------------------

    /// --------------------------------------------------------------------------------
    /// Step 2: Create a ring communicator  
    sub  = gcpe()->world.split((DA.size() > 0 || DB.size() > 0) ? 1 : MPI_UNDEFINED);

    if(0 == sub.size())
    {
        Log::Instance().write("ParallelMatMul: Not involved, skipping.\n");
        goto fn_exit;
    }
    else
        Log::Instance().write("ParallelMatMul: Matrices distributed over %d processes.\n", sub.size());

    ringSize = sub.size();
    ringPer  = 1;
    ring     = mpi::Cartcommunicator(sub, 1, &ringSize, &ringPer);

    left  = ring.neighbour(-1);
    right = ring.neighbour(+1);
    /// --------------------------------------------------------------------------------

    /// --------------------------------------------------------------------------------
    /// Step 3: Perform the matrix multiplication step by step

    /// First we perform the local product
    linalg::CoordMatrix<int64, double>::coomm(alpha, DA, DB, beta, C);

    for(round = 1; round < ringSize; ++round)
    {
        /// Send the size of the matrix B to the 
        /// right
        sendcount = DB.size();
        req = ring.isend(&sendcount, 1, MPI_INT, right, 6);

        /// Receive the value from the left neighbour
        ring.recv(&recvcount, 1, MPI_INT, left, 6);

        mpi::wait(&req);
         
        /// sendrecv_replace allows us to use the same buffer
        /// at the expense of providing the same send and receive
        /// count
        count = std::max(sendcount, recvcount);
        ring.allreduce(MPI_IN_PLACE, &count, 1, MPI_INT, MPI_MAX);
        DB.resize(count);

        ring.sendrecv_replace(&DB[0], count, DB.type(), right, 7, left, 7);

        /// Need to resize the matrix in case recvcount is smaller than
        /// sendcount
        DB.resize(recvcount);

        Log::Instance().write("ParallelMatMul: In round %2d, count = %6d, recvcount = %6d\n", round, count, recvcount);

        linalg::CoordMatrix<int64, double>::coomm(alpha, DA, DB, 1.0, C);
    }
    /// --------------------------------------------------------------------------------

fn_exit:
    gcpe()->world.barrier();
    Log::Instance().write("Finished ParallelMatMul.\n");
}

