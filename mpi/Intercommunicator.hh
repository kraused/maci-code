//
// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#ifndef MACI_MPI_INTERCOMM_HH_INCLUDED
#define MACI_MPI_INTERCOMM_HH_INCLUDED 1

#include "mpi/config.hh"
#include "mpi/Communicator.hh"

namespace mpi {

    /// Wrapper around a MPI intercommunicator
    class Intercommunicator : public Communicator
    {

    public: 
        /// Create a new intercommunicator with the rank zero in the
        /// local groups as masters. The constructor is collective on
        /// the peer communicator
        /// @param peer     peer communicator
        /// @param local    local communicator
        Intercommunicator(const Communicator& peer, const Communicator& local);

        /// Get size of remote group
        /// @returns int > 0
        inline int remotesize() const
        {
            int size;
            MPI_Comm_remote_size(comm_, &size);
            CHECK_POSTCOND_ALWAYS(size > 0);
            return size;
        }

        /// Get constant reference to remote group
        /// @param Group reference to group
        const Group& RemoteGroup() const
        {
            return remote_group;
        }

    private:
        // The remote group
        Group   remote_group;
    };

}

#endif

