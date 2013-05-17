
// vi: tabstop=4:expandtab

#ifndef MACI_UTILS_PARSCAN_HH_INCLUDED
#define MACI_UTILS_PARSCAN_HH_INCLUDED 1

#include "mpi/Communicator.hh"


namespace utils 
{

    /// par_exclusive_scan: Performs an exclusive scan operation where
    ///                     each processing elements contributes one
    ///                     element to the array of size comm.size(). The
    ///                     output array out is available on all processing
    ///                     elements;
    /// This operation is useful to compute the start offsets for global
    /// ids from the number of local ids.
    void par_exclusive_scan(const mpi::Communicator& comm, int in, int* out);

}

#endif

