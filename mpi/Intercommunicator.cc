
// vi: tabstop=4:expandtab

#include "mpi/Intercommunicator.hh"

using namespace mpi;

/// Create an intercommunicator
/// @param world    peer communicator
/// @param local    the local communicator
static Communicator createIntercommunicator(const Communicator& world, const Communicator& local)
{
    CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != local.getHandle());

    Communicator peercomm = world.split((0 == local.myrank()) ? 1 : MPI_UNDEFINED);
    CHECK_DEBUG(0 == peercomm.size() || 2 == peercomm.size(), "invalid peercommunicator");

#ifndef NDEBUG
    int k = peercomm.size();
    world.allreduce(MPI_IN_PLACE, &k, 1, MPI_INT, MPI_SUM);
    CHECK_DEBUG(k == 4, "invalid peercommunicator");
#endif

    /// Automatically free the communicator so that we can
    /// return Communicator(icomm.c) without leaking a
    /// handle
    struct T {
        ~T() 
        {
            mpi_comm_free(&c); 
        }
        MPI_Comm c;
    } icomm;

    int err = MPI_Intercomm_create(local.getHandle(), 0, peercomm.getHandle(), 
                (peercomm.myrank() + 1)%2, __LINE__, &icomm.c);
    CHECK_ALWAYS(!err, "MPI_Intercomm_create failed"); 

    CHECK_POSTCOND_ALWAYS(MPI_COMM_NULL != icomm.c);

    return Communicator(icomm.c);
}

/// Create remote group
/// @param comm the intercommunicator
/// @returns    the remote group
static Group createRemoteGroup(const Communicator& comm)
{
    int err;

    CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm.getHandle());
    CHECK_PRECOND_ALWAYS(0 <= comm.myrank() && comm.myrank() < comm.size());

    struct T {
        ~T() 
        {
            mpi_group_free(&g); 
        }
        MPI_Group g;
    } group;
    err = MPI_Comm_remote_group(comm.getHandle(), &group.g);
    CHECK_ALWAYS(!err, "MPI call failed"); 

#ifndef NDEBUG
    int sz;
    err = MPI_Comm_remote_size(comm.getHandle(), &sz);
    CHECK_ALWAYS(!err, "MPI call failed"); 
    CHECK_ALWAYS(0 < sz, 0);

    int size;
    MPI_Group_size(group.g, &size);
    CHECK_ALWAYS(size > 0, 0);

    CHECK_ALWAYS(sz == size, 0);
#endif

    return Group(group.g);
}

Intercommunicator::Intercommunicator(const Communicator& peer, 
                     const Communicator& local)
: Communicator(createIntercommunicator(peer, local)),
  remote_group(createRemoteGroup(*this))
{
}

