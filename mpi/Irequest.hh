//
// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#ifndef MACI_MPI_IREQUEST_HH_INCLUDED
#define MACI_MPI_IREQUEST_HH_INCLUDED 1

#include "mpi/config.hh"

namespace mpi 
{

    /// A request for nonblocking communication
    typedef MPI_Request Irequest;

    /// Wait for a request
    /// @param req  the request
    void wait(Irequest *req);
    
    /// Wait for a list of requests
    /// @param reqs request array
    /// @param n    length of request array
    void waitall(Irequest *reqs, int n);

}

#endif

