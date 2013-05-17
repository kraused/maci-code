//
// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#include "mpi/Irequest.hh"

using namespace mpi;

void mpi::wait(Irequest *req)
{
    CHECK_PRECOND_ALWAYS(req);

    int err = MPI_Wait(req, MPI_STATUS_IGNORE);
    CHECK_ALWAYS(!err, "MPI call failed");
}
    
void mpi::waitall(Irequest *reqs, int n)
{
    CHECK_PRECOND_ALWAYS((n == 0) || reqs);
    CHECK_PRECOND_ALWAYS(n >= 0);

    if(n == 0) {
        return;
    }

    int err = MPI_Waitall(n, reqs, MPI_STATUSES_IGNORE);
    CHECK_ALWAYS(!err, "MPI call failed");
}

