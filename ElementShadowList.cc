
// vi: tabstop=4:expandtab

#include "ElementShadowList.hh"
#include "Logging.hh"
#include "ProcElement.hh"
#include "Logging.hh"
#include "utils/strprintf.hh"
#include "mpi/Intercommunicator.hh"


ElementShadowList::ElementShadowList()
: ellist(0)
{

}

ElementShadowList::ElementShadowList(ElementList& list)
: ellist(&list)
{
    int i;
    
    i = 0;
    for(ElementList::ConstIterator it = list.begin(); it != list.end(); ++it)
    {
        ElementShadow elsh;

        InitializeElementShadow(*it, i++, &elsh);
        Base::push_back(elsh);
    }
}

void ElementShadowList::Exchange()
{
    int p, sendcount, *recvcounts, *recvdispls, remoteSize;
    ElementShadow dummy;

//    /// Build an intercommunicator connecting mesh and particle
//    /// processing elements
//    mpi::Intercommunicator intercomm(gcpe()->world, gcpe()->myscale);
//    remoteSize = intercomm.RemoteGroup().size();
//
//    /// --------------------------------------------------------------------------------
//    /// Step 1: Exchange the sendcount and compute the displacements
//    recvcounts = new int[remoteSize];
//    recvdispls = new int[remoteSize+1];
//
//    sendcount = size();
//    intercomm.allgather(&sendcount, 1, MPI_INT, recvcounts, 1, MPI_INT);
//    Log::Instance().write("ElementShadowList recvcounts = %s\n", utils::list2str("%d", recvcounts, recvcounts+remoteSize).c_str());
//        
//    recvdispls[0] = 0;
//    for(p = 1; p <= remoteSize; ++p)
//        recvdispls[p] = recvdispls[p-1] + recvcounts[p-1];
//    /// --------------------------------------------------------------------------------
//  
//    /// --------------------------------------------------------------------------------
//    /// Step 2: Exchange data data
//    if(recvdispls[remoteSize] > 0)
//    {
//        CHECK_ALWAYS(PROC_MD == gcpe()->mytype, "FE processing elements should not receive data");
//        CHECK_ALWAYS(0 == sendcount, "Only particle processing elements should receive data");
//
//        resize(recvdispls[remoteSize]);
//        /// In principle the send argument doesn't matter on 
//        /// those processing elements which do not send anything
//        /// However, I have seen problems with passing NULL or
//        /// an invalid type to a function. Therefore we just provide
//        /// a dummy argument in the knowledge that it is never
//        /// actually referenced.
//        intercomm.allgatherv(&dummy, sendcount, ElementShadow::type(), &(*this)[0], recvcounts, recvdispls, ElementShadow::type());
//
//        Log::Instance().write("Number of local shadow elements = %lu\n", size());
//    }
//    else
//    {
//        CHECK_ALWAYS(PROC_FE == gcpe()->mytype, "Only MD processing elements should receive data");
//        intercomm.allgatherv(&(*this)[0], sendcount, ElementShadow::type(), &dummy, recvcounts, recvdispls, ElementShadow::type());
//    }
//    /// --------------------------------------------------------------------------------

    /// TODO With the Open MPI 1.4.2 version on cub, the intercommunication doesn't work
    ///      It either hangs in the allgatherv or, if I replace allgatherv by alltoallv
    ///      calls, I see hangs in the intercomm destructor. This should be sorted out
    ///      at some point
    int *sendcounts, *senddispls;
    int worldSize;

    worldSize  = gcpe()->world.size();
    remoteSize = worldSize - gcpe()->myscale.size();

    /// --------------------------------------------------------------------------------
    /// Step 1: Exchange the sendcount and compute the displacements
    recvcounts = new int[worldSize];
    recvdispls = new int[worldSize + 1];

    sendcounts = new int[worldSize];
    senddispls = new int[worldSize];

    sendcount  = size();

    std::fill(sendcounts, sendcounts + worldSize, 0);
    /// Here we use that the MD processing elements have rank 0,...,#(MD procs)-1
    if(PROC_FE == gcpe()->mytype)
    {
        for(p = 0; p < remoteSize; ++p)
            sendcounts[p] = sendcount;
    }

    gcpe()->world.alltoall(sendcounts, 1, MPI_INT, recvcounts, 1, MPI_INT);

    std::fill(senddispls, senddispls + worldSize, 0);

    recvdispls[0] = 0;
    for(p = 1; p <= worldSize; ++p)
        recvdispls[p] = recvdispls[p-1] + recvcounts[p-1];

    Log::Instance().write("ElementShadowList recvcounts = %s\n", utils::list2str("%d", recvcounts, recvcounts+worldSize).c_str());
    
    if(recvdispls[worldSize] > 0)
    {
        CHECK_ALWAYS(PROC_MD == gcpe()->mytype, "FE processing elements should not receive data");
        CHECK_ALWAYS(0 == sendcount, "Only particle processing elements should receive data");

        resize(recvdispls[worldSize]);
        /// In principle the send argument doesn't matter on 
        /// those processing elements which do not send anything
        /// However, I have seen problems with passing NULL or
        /// an invalid type to a function. Therefore we just provide
        /// a dummy argument in the knowledge that it is never
        /// actually referenced.
        gcpe()->world.alltoallv(&dummy, sendcounts, senddispls, ElementShadow::type(), &(*this)[0], recvcounts, recvdispls, ElementShadow::type());

        Log::Instance().write("Number of local shadow elements = %lu\n", size());
    }
    else
    {
        CHECK_ALWAYS(PROC_FE == gcpe()->mytype, "Only MD processing elements should receive data");
        gcpe()->world.alltoallv(&(*this)[0], sendcounts, senddispls, ElementShadow::type(), &dummy, recvcounts, recvdispls, ElementShadow::type());
    }
    /// --------------------------------------------------------------------------------

    delete[] sendcounts;
    delete[] senddispls;


    gcpe()->world.barrier();
    Log::Instance().write("Finished exchanging the shadow grid.\n");

    delete[] recvcounts;
    delete[] recvdispls;
}

