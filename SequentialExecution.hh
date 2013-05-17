
// vi: tabstop=4:expandtab

#ifndef MACI_SEQUENTIAL_EXECUTION_HH_INCLUDED
#define MACI_SEQUENTIAL_EXECUTION_HH_INCLUDED 1

#include "mpi/Communicator.hh"

/// Execute a function in sequence
/// @param comm the communicator. The ranks define an ordering of the nodes
/// @param f    the functor
template<typename F>
void executeSequential(const mpi::Communicator& comm, const F& f)
{
    for(int i = 0; i < comm.size(); ++i) {
        if(i == comm.myrank()) {
            f();
        }
        comm.barrier();
    }
}

#endif


