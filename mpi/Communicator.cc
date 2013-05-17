
// vi: tabstop=4:expandtab

#include "mpi/Communicator.hh"

using namespace mpi;

Communicator Communicator::split(int color) const   
{
    CHECK_PRECOND_DEBUG(size() > 0 || MPI_COMM_NULL == comm_);
    if(0 == size()) {       
        return Communicator(MPI_COMM_NULL);
    }

    struct T {
        ~T() 
        {
            mpi_comm_free(&c);
        }
        MPI_Comm c;
    } w;
    MPI_Comm_split(comm_, color, myrank(), &w.c);
    
    return Communicator(w.c);
}

