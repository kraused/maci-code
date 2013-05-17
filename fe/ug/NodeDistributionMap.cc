
// vi: tabstop=4:expandtab

#include "NodeDistributionMap.hh"
#include "Mesh.hh"
#include "Solver.hh"


using namespace fe::interface;


NodeDistributionMap::NodeDistributionMap(Mesh* M)
: Base(M), mesh(M), perank(0)
{
}

void NodeDistributionMap::CreateMappingFromPriorities(Solver& solver, int rank, NodalVector<int>& P)
{
    perank = rank;

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        (*this)(nd) = (((int64_t )P(nd)) << 32) | rank;
    /* Warning: ParallelMax takes an instance of NodeDistributionMap as input (for 
            consistency) but doesn't use it. Hence, for now passing NULL as the
            second argument is fine */
    this->ParallelMax(solver, NULL);

    // Remove the first 32 bits from the values
    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        (*this)(nd) = (int )(*this)(nd);

#if 1 && !defined(NDEBUG)
    int Min = perank;
    int Max = perank;
    // Use the internal UG communicator here
    MPI_Allreduce(MPI_IN_PLACE, &Max, 1, MPI_INT, MPI_MAX, solver.GetCommunicator());
    MPI_Allreduce(MPI_IN_PLACE, &Min, 1, MPI_INT, MPI_MIN, solver.GetCommunicator());

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
    {
        ASSERT((*this)(nd) >= Min);
        ASSERT((*this)(nd) <= Max);
    }

    // Count the number of copies of a node
    NodalVector<int> t(mesh);
    for(fe::interface::Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        t(nd) = 1;
    t.ParallelSumup(solver, this);

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
    {
        if(1 == t(nd))
            ASSERT(perank == (*this)(nd));
        if(perank != (*this)(nd))
            ASSERT(t(nd) > 1);
    }

    for(fe::interface::Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        t(nd) = perank;
    t.ParallelCopy(solver, this);

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        ASSERT(t(nd) == (*this)(nd));

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        t(nd) = bOwn(nd) ? 1 : 0;
    t.ParallelCopy(solver, this);

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        ASSERT(1 == t(nd));

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        t(nd) = bOwn(nd) ? 1 : 0;
    t.ParallelSumup(solver, this);

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        ASSERT(1 == t(nd));
#endif
}

