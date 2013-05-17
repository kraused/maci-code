
// vi: tabstop=4:expandtab

#include "TreeAssemblerIntermediateFormat.hh"
#include "DesignByContract.hh"
#include "Logging.hh"
#include "mpi/Datatype.hh"


/// Construct a datatype for TreeAssemblerIntermediateFormat::Entry
/// @param type     on exit: a commited derived datatype representing the
///         the entry structure
template<typename Tp>
static void ConstructDatatype(MPI_Datatype *type)
{
    (*type) = MPI_DATATYPE_NULL;
    CHECK_POSTCOND_ALWAYS(MPI_DATATYPE_NULL != (*type));
}

/// @specialization
template<>
void ConstructDatatype<float>(MPI_Datatype *type)
{
    /// TODO In MPI 2.2 there is MPI_INT64_T
    create_struct4(*type, TreeAssemblerIntermediateFormat<float>::Entry,
                   elementIndex, MPI_INT   , 1,
                   cornerIndex , MPI_INT   , 1,
                   atomIndex   , MPI_LONG_LONG_INT, 1,
                   val         , MPI_FLOAT , 1);
    MPI_Type_commit(type);

    CHECK_POSTCOND_ALWAYS(MPI_DATATYPE_NULL != (*type));
}

/// @specialization
template<>
void ConstructDatatype<double>(MPI_Datatype *type)
{
    /// TODO In MPI 2.2 there is MPI_INT64_T
    create_struct4(*type, TreeAssemblerIntermediateFormat<double>::Entry,
                   elementIndex, MPI_INT   , 1,
                   cornerIndex , MPI_INT   , 1,
                   atomIndex   , MPI_LONG_LONG_INT, 1,
                   val         , MPI_DOUBLE, 1);
    MPI_Type_commit(type);

    CHECK_POSTCOND_ALWAYS(MPI_DATATYPE_NULL != (*type));
}

void ExchangeListSizes(mpi::Communicator& comm, int len, int* sizes)
{
    comm.allgather(&len, 1, MPI_INT, sizes, 1, MPI_INT);
}

template<typename Tp>
void TreeAssemblerIntermediateFormat<Tp>::Exchange()
{
    MPI_Datatype type;
    int k, p, *sendcounts, *recvcounts, *senddispls, *recvdispls, worldSize;
    Entry *sendbuf, *recvbuf;
    std::size_t i;

    ConstructDatatype<Tp>(&type);
    
    worldSize = gcpe()->world.size();

    /// --------------------------------------------------------------------------------
    /// Step 1: Gather send and receive counts
    sendcounts = new int[worldSize]();
    recvcounts = new int[worldSize];
    senddispls = new int[worldSize+1];
    recvdispls = new int[worldSize+1];

    if(PROC_MD == gcpe()->mytype)
    {
        k = 0;
        for(ElementShadowList::ConstIterator it = list.begin(); it != list.end(); ++it, ++k)
            sendcounts[it->owner] += entries[k].size();
    }
    
    gcpe()->world.alltoall(sendcounts, 1, MPI_INT, recvcounts, 1, MPI_INT);
   
    Log::Instance().write("TreeAssemblerIntermediateFormat::Exchange() recvcounts = %s\n", utils::list2str("%d", recvcounts, recvcounts+worldSize).c_str());
    
    senddispls[0] = 0;
    for(p = 1; p <= worldSize; ++p)
        senddispls[p] = senddispls[p-1] + sendcounts[p-1];

    recvdispls[0] = 0;
    for(p = 1; p <= worldSize; ++p)
        recvdispls[p] = recvdispls[p-1] + recvcounts[p-1];
    /// --------------------------------------------------------------------------------

    /// --------------------------------------------------------------------------------
    /// Step 2: Exchange the data
    sendbuf = new Entry[senddispls[worldSize]];
    recvbuf = new Entry[recvdispls[worldSize]];

    if(PROC_MD == gcpe()->mytype)   
    {
        k = 0;
        for(ElementShadowList::ConstIterator it = list.begin(); it != list.end(); ++it, ++k)
            for(i = 0; i < entries[k].size(); ++i)
            {
                sendbuf[senddispls[it->owner]].elementIndex = entries[k][i].elementIndex;
                sendbuf[senddispls[it->owner]].cornerIndex  = entries[k][i].cornerIndex;

                /// Make the atom index globally unique by prepending
                /// the rank in the world communicator
                CHECK_ALWAYS(0 == Int64::hi(entries[k][i].atomIndex), 0);
                sendbuf[senddispls[it->owner]].atomIndex = Int64::concat(gcpe()->world.myrank(), entries[k][i].atomIndex);
            
                sendbuf[senddispls[it->owner]++].val = entries[k][i].val;
            }
    }

    /// We need to recompute the senddispls because we used them for filling
    /// up the sendbuf
    senddispls[0] = 0;
    for(p = 1; p < worldSize; ++p)
        senddispls[p] = senddispls[p-1] + sendcounts[p-1];

    gcpe()->world.alltoallv(sendbuf, sendcounts, senddispls, type,
                            recvbuf, recvcounts, recvdispls, type);
    /// --------------------------------------------------------------------------------

    /// --------------------------------------------------------------------------------
    /// Step 3: Insert the data
    if(PROC_MD == gcpe()->mytype)
        CHECK_ALWAYS(0 == recvdispls[worldSize], "Should not happen.");
   
    for(k = 0; k < recvdispls[worldSize]; ++k)
        insert(recvbuf[k]);
    /// --------------------------------------------------------------------------------

    delete[] sendbuf;
    delete[] recvbuf;
    delete[] recvdispls;
    delete[] senddispls;
    delete[] recvcounts;
    delete[] sendcounts;

    MPI_Type_free(&type);
}


/// Explicitly instantiate the class. This is necessary because we
/// implement some functions in the .cc file. If we are going to use
/// more types they also need to be instantiated here.
template class TreeAssemblerIntermediateFormat<double>;

