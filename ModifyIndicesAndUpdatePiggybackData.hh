
// vi: tabstop=4:expandtab

#ifndef MACI_MODIFY_INDICES_AND_UPDATE_PIGGYBACK_DATA
#define MACI_MODIFY_INDICES_AND_UPDATE_PIGGYBACK_DATA 1

#include <utility>
#include <vector>

#include "ProcElement.hh"
#include "ParticleList.hh"
#include "linalg/CoordMatrix.hh"
#include "Int64.hh"
#include "mpi/Communicator.hh"


/// IntPair: A pair of two integers
typedef std::pair<int,int> IntPair;

/// GatherUniqueColumnIndices: Gather the list of column indices in the matrix A.
void GatherUniqueColumnIndices(linalg::CoordMatrix<int64,double>& A, std::vector<int64>* columnIndices);

/// LocalIndex: Map a global index to a local one by finding its position in
///             columnIndex.
/// This function has linear complexity.
static inline int LocalIndex(std::vector<int64> columnIndices, int64 index)
{
    std::vector<int64>::const_iterator it;

    it = std::find(columnIndices.begin(), columnIndices.end(), index);
    CHECK_ALWAYS(it != columnIndices.end(), "Should not happen.");

    return it - columnIndices.begin();
}

/// ModifyIndicesAndUpdatePiggybackData: Performs the transformation of
/// an assembled [nodes] x [atoms] or [atoms] x [atoms] matrix into it's
/// final form. To make the SpMV product work, the piggyback data must also
/// be updated.
///
/// When the transfer matrix (or damping matrix) has been assembled we need
/// to create a more suitable representation of the matrix. It is inefficient
/// to use global particle indices because the required storage for the input
/// to SpMV is equal to the biggest column index. Hence we compress the matrix
/// by creating a local indexing for the unique column indices. These local
/// displacements must be communicated back to the particle processing elements
/// owning the particles and scattered into the piggyback datastructure using
/// the scat functor which implements
///
///     void operator()(PiggybackType* p, int rank, int displacement);
///
template<typename Scatterer>
void ModifyIndicesAndUpdatePiggybackData(ParticleList& list, mpi::Communicator& comm, Scatterer scat, linalg::CoordMatrix<int64,double>* A)
{
    std::size_t i;
    std::vector<int64> columnIndices;
    int p, k, rank, *sendcounts, *recvcounts, *senddispls, *recvdispls, worldSize;
    IntPair *sendbuf, *recvbuf;

    worldSize = gcpe()->world.size();

    /// ----------------------------------------------------------------------------------------------------
    /// Step 1: Compute the list of unique column indices. Here the indices are still 64 bit.
    GatherUniqueColumnIndices(*A, &columnIndices);
    /// ----------------------------------------------------------------------------------------------------

    /// ----------------------------------------------------------------------------------------------------
    /// Step 2: Create and exchange the buffer
    sendcounts = new int[worldSize]();
    recvcounts = new int[worldSize];
    senddispls = new int[worldSize+1];
    recvdispls = new int[worldSize+1];
    
    for(i = 0; i < columnIndices.size(); ++i)
        sendcounts[Int64::hi(columnIndices[i])] += 1;

    senddispls[0] = 0;
    for(p = 0; p <= worldSize; ++p)
        senddispls[p] = senddispls[p-1] + sendcounts[p-1];

    gcpe()->world.alltoall(sendcounts, 1, MPI_INT, recvcounts, 1, MPI_INT);

    recvdispls[0] = 0;
    for(p = 0; p <= worldSize; ++p)
        recvdispls[p] = recvdispls[p-1] + recvcounts[p-1];

    sendbuf = new IntPair[senddispls[worldSize]];
    recvbuf = new IntPair[recvdispls[worldSize]];

    for(i = 0; i < columnIndices.size(); ++i)
        sendbuf[senddispls[Int64::hi(columnIndices[i])]++] = IntPair(Int64::lo(columnIndices[i]), i);

    /// We need to recompute the senddispls because we used them for filling
    /// up the sendbuf
    senddispls[0] = 0;
    for(p = 0; p < worldSize; ++p)
        senddispls[p] = senddispls[p-1] + sendcounts[p-1];

    gcpe()->world.alltoallv(sendbuf, sendcounts, senddispls, MPI_2INT,
                            recvbuf, recvcounts, recvdispls, MPI_2INT);
    /// ----------------------------------------------------------------------------------------------------

    /// ----------------------------------------------------------------------------------------------------
    /// Step 3: Scatter the indices to the particles. This is done by calling the scat() functor for
    ///         the  Create and exchange the buffer
    for(p = 0; p < worldSize; ++p)
    {
        /// Translate the rank in world to the comm
        rank = mpi::Group::translate(gcpe()->world, p, comm);

        for(k = recvdispls[p]; k < recvdispls[p+1]; ++k)
            scat(md::interface::Property::Piggyback(*gcpe()->GetMMSystem(), list[recvbuf[k].first]), rank, recvbuf[k].second);
    }
    /// ----------------------------------------------------------------------------------------------------

    /// ----------------------------------------------------------------------------------------------------
    /// Step 4: Modify the column indices of A, replacing the old column indices with the local ondes
    for(i = 0; i < A->size(); ++i)
    {
        /// This is an algorithm with quadratric complexity. We could either try
        /// to make the search cheaper (tree?) or compute an inverse mapping from
        /// the global (particle) index to the local index but this could be memory
        /// intensive when particle arrays are highly scattered.
        (*A)[i].col = LocalIndex(columnIndices, (*A)[i].col);

        /// The matrix should be distributed by row, so we should 
        /// have only entries with highest 32-bit equal to the rank
        /// of this processing element
        CHECK_ALWAYS(Int64::hi((*A)[i].row) == gcpe()->world.myrank(), "Should not happen.");
        (*A)[i].row = Int64::lo((*A)[i].row);
    }
    /// ----------------------------------------------------------------------------------------------------

    delete[] sendbuf;
    delete[] recvbuf;
    delete[] recvdispls;
    delete[] senddispls;
    delete[] recvcounts;
    delete[] sendcounts;
}

#endif

