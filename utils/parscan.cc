
// vi: tabstop=4:expandtab

#include "utils/parscan.hh"


void utils::par_exclusive_scan(const mpi::Communicator& comm, int in, int* out)
{
    int val0, val1, val2, i;

    out[comm.myrank()] = in;
    comm.allgather(MPI_IN_PLACE, 1, MPI_INT, out, 1, MPI_INT);

    val0 = 0;
    val1 = out[0];

    out[0] = 0;
    for(i = 1; i < comm.size(); ++i)
    {
        val2  = val0 + val1;
        val0  = val2;
        val1  = out[i];
        
        out[i] = val2;
    }
}

