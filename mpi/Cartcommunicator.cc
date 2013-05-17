
// vi: tabstop=4:expandtab

#include "mpi/Cartcommunicator.hh"

using namespace mpi;

Cartcommunicator::Cartcommunicator()
{
}

Cartcommunicator::Cartcommunicator(const Cartcommunicator& other)
: Communicator(other)
{
}

Cartcommunicator& Cartcommunicator::operator=(const Cartcommunicator& cart)
{
    dimension = cart.dimension;
    Communicator::operator=(cart);

    return *this;
}

Cartcommunicator::Cartcommunicator(MPI_Comm comm, int dim, 
                 const int dims[], const int periods[],
                 int reorder)
: dimension(dim)
{
    initialize(comm, dim, dims, periods, reorder);
}

Cartcommunicator::Cartcommunicator(const Communicator& comm, int dim, 
                 const int dims[], const int periods[],
                 int reorder)
: dimension(dim)
{
    initialize(comm.getHandle(), dim, dims, periods, reorder);
}

int Cartcommunicator::neighbour(int sx, int sy, int sz) const
{
    int err;
    CHECK_PRECOND_ALWAYS(3 >= dimension);

    int coords[3], periods[3], dims[3];
    err = MPI_Cart_get(Communicator::comm_, dimension, 
                   dims, periods, coords);
    CHECK_ALWAYS(!err, "MPI call failed");

    int s[] = { sx, sy, sz };
    for(int d = 0; d < dimension; ++d) {
        coords[d] += s[d];
        if(!periods[d]  && (coords[d] < 0 || coords[d] >= dims[d])) {
            return MPI_PROC_NULL;
        }
    }

    int rank;
    err = MPI_Cart_rank(Communicator::comm_, coords, &rank);
    CHECK_ALWAYS(!err, "MPI call failed");

    CHECK_POSTCOND_ALWAYS(0 <= rank && rank < size());

    return rank;
}

void Cartcommunicator::initialize(MPI_Comm comm, int dim, 
                 const int dims[], const int periods[],
                 int reorder)
{
    int err;
    CHECK_PRECOND_ALWAYS(3 >= dim);

    MPI_Comm cartcomm;
    err = MPI_Cart_create(comm, dim, const_cast<int*>(dims), 
            const_cast<int*>(periods), reorder, &cartcomm);
    CHECK_ALWAYS(!err, "MPI call failed");
    Communicator::initialize(cartcomm);
    err = MPI_Comm_free(&cartcomm);
    CHECK_ALWAYS(!err, "MPI call failed");
}

