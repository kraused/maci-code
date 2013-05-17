//
// Copyright (c) 2010 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#ifndef MACI_MPI_CONFIG_HH_INCLUDED
#define MACI_MPI_CONFIG_HH_INCLUDED 1

#include <mpi.h>
#include <string.h>

/** We use the design by contract functionality of the main code as long as
 *  there is no seperate library for this
 */
#include "DesignByContract.hh"

/** Evaluate a command only if the MPI library is not yet finalized
 */
#define eval_if_not_finalized(cmd)      \
    do {                    \
        int _mangle_flag;       \
        MPI_Finalized(&_mangle_flag);   \
        if(!_mangle_flag) {     \
            cmd;            \
        }               \
    } while(0)

/** Wrapper around MPI_Comm_free
 */
static inline void mpi_comm_free(MPI_Comm *comm)
{
    eval_if_not_finalized( if(MPI_COMM_NULL != *comm) {     \
                    MPI_Comm_free(comm);    \
                } );
}

/** Wrapper around MPI_Group_free
 */
static inline void mpi_group_free(MPI_Group *group)
{
    eval_if_not_finalized( if(MPI_GROUP_NULL != *group) {   \
                    MPI_Group_free(group);  \
                } );
}

#endif

