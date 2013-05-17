//
// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#include "mpi/Group.hh"

using namespace mpi;

Group Group::operator-(const Group& grp)
{
    struct T {
        ~T() 
        {
            mpi_group_free(&g);
        }
        MPI_Group g;
    } w;
    MPI_Group_difference(grp_, grp.grp_, &w.g);

    return Group(w.g);
}

