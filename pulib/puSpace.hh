
// vi: tabstop=4:expandtab

#ifndef PULIB_SPACE_HH_INCLUDED
#define PULIB_SPACE_HH_INCLUDED 1

#include "puTree.hh"
#include "puBasisFunction.hh"
#include "pulib/typedefs.hh"
#include "puParallel.hh"

namespace pulib
{

// The PU approximation space; This class
// combines instancez of the different classes
// necessary to build a PU
template<int D>
struct ApproximationSpace
{
    typedef typename Types<D>::Box      Box;
    typedef typename Types<D>::double3  double3;
    
public:
    // Construct a new approximation space
    // @pt and @hJ can be deleted after the constructor returns
    ApproximationSpace(MPI_Comm comm, const int nbranks[],
               const Box& boundbox, double3* pt, 
               double3* hJ, int* idx, int N);
               
    // Number of neighbors for a processing element
    static const int NBNEIGHBORS = GlobalTree<D>::NBNEIGHBORS;
            
    // Delete the approximation space
    ~ApproximationSpace();

    // Get the number of generating functions; In the parallel
    // case this is a synchronizing function
    int getSizeBasis() const;
    
    // Get the number of local basis functions
    inline int getSizeLocalBasis() const
    {
        return B_.size();
    }

    // Query the PU tree
    inline void queryTreeGlobal(pulib::Query<D>& query)
    {
        tree_.queryAll(query);
    }
    inline void queryTreeLocal(pulib::Query<D>& query)
    {
        tree_.queryLocal(query);
    }
    
    // Iterators for generating functions
    typedef typename Basis<D>::Iterator     Iterator;
    typedef typename Basis<D>::ConstIterator    ConstIterator;

    // Get the first iterator
    inline Iterator begin()
    {
        return B_.begin();
    }
    inline ConstIterator begin() const
    {
        return B_.begin();
    }   
    
    // Get the last iterator (end)
    inline Iterator end()
    {
        return B_.end();
    }
    inline ConstIterator end() const
    {
        return B_.end();
    }   
    
    // Get iterator by index; See the comment in @puBasisFunction.hh 
    // concerning total indices
    inline Iterator getFunction(int id)
    {
        return B_.getFunction(id);
    }
    inline ConstIterator getFunction(int id) const
    {
        return B_.getFunction(id);
    }
    
private:
    // The cartesian communicator
    MPI_Comm    cartcomm_;
    // Bounding box of all particles
    Box     boundbox_;
    // The local part of the generating system; In the
    // parallel case the combination of these systems
    // from different nodes yields the generating system
    // of the space
    Basis<D>    B_;
    // The partition of unity tree
    GlobalTree<D>   tree_;
};

template<int D>
ApproximationSpace<D>::ApproximationSpace(MPI_Comm   comm, 
                      const int  nbranks[],
                      const Box& boundbox, 
                      double3*   pt, 
                      double3*   hJ, 
                      int*       idx, 
                      int        N)
: cartcomm_(MPI_COMM_NULL), boundbox_(boundbox), B_(pt, hJ, idx, N), 
  tree_(boundbox, B_, comm, nbranks)
{
    ASSERT(MPI_COMM_NULL != comm);
    MPI_Comm_dup(comm, &cartcomm_);
}

template<int D>
ApproximationSpace<D>::~ApproximationSpace()
{
    int stat;
    MPI_Finalized(&stat);

    if(!stat) {
        MPI_Comm_free(&cartcomm_);
    }
}

template<int D>
int ApproximationSpace<D>::getSizeBasis() const
{
    ASSERT(MPI_COMM_NULL != cartcomm_);

    int size = getSizeLocalBasis();
    MPI_Allreduce(MPI_IN_PLACE, &size, 1, MPI_INT, MPI_SUM, cartcomm_);
    
    return size;
}

}

#endif

