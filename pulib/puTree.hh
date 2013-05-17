
// vi: tabstop=4:expandtab

#ifndef PULIB_TREE_HH_INCLUDED
#define PULIB_TREE_HH_INCLUDED 1

#include "treelib/treelib.hh"
#include "puPatch.hh"
#include "puBasisFunction.hh"
#include "puParallel.hh"
#include "pulib/typedefs.hh"

namespace pulib
{

// The (local) partition of unity tree
template<int D>
class Tree : public treelib::OrthRangeTree<D,Patch<D>/**/>
{
    
public:
    typedef typename treelib::OrthRangeTree<D,Patch<D>/**/> Base;
    typedef Patch<D>                    AssocData;
    
public:
    // Construct a new tree given a bounding box
    Tree(const typename Base::Box& box);
    
};

// Query for a partition of unity tree
template<int D>
class Query : public treelib::RangeTreeQuery<D,Patch<D>/**/>
{

};

// The global partition of unity tree
// Knows about parallelism
template<int D>
class GlobalTree
{
public:
    typedef typename Types<D>::double3              double3;
    typedef typename treelib::OrthRangeTree<D,Patch<D>/**/>::Box    Box;
    typedef treelib::HierarchConstr<D,typename Tree<D>::AssocData>  HierarchConstr;

    // Construct a new global tree with local box @locBox
    // Note that we do not have to care about the global bounding
    // box since the queries are always local
    // The construct inserts all particles from @S and
    // exchanges halos
    // @param nbranks   array of length NBNEIGHBORS+1 containing the ranks
    //          of all neighbors (and the own rank) in
    //          lexicographical row-major ordering. To indicate
    //          a missing neighbor MPI_COMM_NULL should be used.
    //          Neighbor relation ship must not be symmetric,
    //          however the bounding boxes of neighbor processors
    //          are assumed to match
    GlobalTree(const Box& locBox, Basis<D>& S, 
        MPI_Comm comm,  const int nbranks[]);
    
    // Clean up
    ~GlobalTree();
    
    // Query all trees
    void queryAll(Query<D>& query);

    // Query local tree
    inline void queryLocal(Query<D>& query)
    {
        if(subTrees_[1][1][1]) {
            subTrees_[1][1][1]->queryTree(query);
        }
    }

    // Returns the number of all superpatches
    // Can't be const because queries are used to get the
    // numbers and there are no ConstQueries ...
    int getNumSuperpatches();

    // Construct on all trees
    void constructOnAll(HierarchConstr& recipe);

    // The number of neighbors of a processing element is 8 in two dimensions
    // and 26 in 3 dimensions
    static const int    NBNEIGHBORS = 2*(9*D - 14);

private:
    // The subtrees
    Tree<D>*        subTrees_[3][3][3];
    // The communicator used for 
    MPI_Comm        comm_;
    // An arena for the remote patches
    treelib::Arena*     remotePatches_;
    // An arena for the superpatches
    treelib::Arena*     superPatches_;
    // The neighbor list
    int         nbranks_[NBNEIGHBORS+1];

};


template<int D>
Tree<D>::Tree(const typename Base::Box& box)
: Base(box)
{
}

// Insert local particles in tree
template<int D, typename I>
void insertLocalParticles(Tree<D>& tree, I first, I last)
{
    for(; first != last; ++first) { 
        tree.insert(first->getParticlePt(), *first);
    }
}

// Get global maximal patch sidelength
template<int D>
void globalMaxPatchSidelength(const Basis<D>& S, MPI_Comm comm, 
        typename GlobalTree<D>::double3 hJ);

// Insert remote particles in the tree
template<int D>
void insertRemoteParticles(Tree<D>* tree[3][3][3], 
        const typename GlobalTree<D>::Box& box, const double* hJ, 
        MPI_Comm comm, const int nbranks[], treelib::Arena** arena);

// Construct the superpatches for intermediate tree nodes
template<int D>
treelib::Arena* buildSuperPatches(GlobalTree<D>* T);

// Construct the list of all overlapping patches for
// all local patches
template<int D>
void queryConnectivity(GlobalTree<D>* T, Basis<D>& S);

// Macros which allows to iterator over all cells
// of the 3x3x3 cells
#undef  forall2
#define forall2(i,j)                    \
    for(int (i) = 0; (i) < 3; ++(i))        \
        for(int (j) = 0; (j) < 3; ++(j))

#undef  forall3
#define forall3(i,j,k)                  \
    for(int (i) = 0; (i) < 3; ++(i))        \
        for(int (j) = 0; (j) < 3; ++(j))    \
            for(int (k) = 0; (k) < 3; ++(k))

template<int D>
GlobalTree<D>::GlobalTree(const Box& locBox, Basis<D>& S,
        MPI_Comm comm, const int nbranks[])
: remotePatches_(0), superPatches_(0)
{
    int numprocs;

    forall3(i,j,k) {
        subTrees_[i][j][k] = 0;
    }

    MPI_Comm_dup (comm, &comm_);
    MPI_Comm_size(comm, &numprocs);

    std::fill(nbranks_, nbranks_+NBNEIGHBORS+1, MPI_PROC_NULL);

    // If the basis is empty we do not create the tree itself since this
    // simplifies the usage of the tree: The user can pass invalid
    // communicators or similar.    
    if(0 == S.size()) {
        std::fill(nbranks_, nbranks_+NBNEIGHBORS+1, 0);
        return;
    }

    std::copy(nbranks, nbranks+NBNEIGHBORS+1, nbranks_);
    for(int p = 0; p <= NBNEIGHBORS; ++p ) {
        if(nbranks_[p] == MPI_PROC_NULL)
            continue;
        if(nbranks_[p] < 0 || nbranks_[p] >= numprocs) {
            fprintf(stderr, " ERROR: nbranks[%d] = %d should be in range 0-%d.\n", p, nbranks_[p], numprocs);
            abort();
        }
    }


    subTrees_[1][1][1] =  new Tree<D>(locBox);  
    insertLocalParticles(*subTrees_[1][1][1], S.begin(), S.end());
    
    double3 hJ;
    globalMaxPatchSidelength(S, comm_, hJ);
    
    insertRemoteParticles(subTrees_, locBox, hJ, comm_, nbranks_, 
                          &remotePatches_);
    
    superPatches_ = buildSuperPatches(this);
    
    queryConnectivity(this, S);
}

template<int D>
GlobalTree<D>::~GlobalTree()
{
    forall3(i,j,k) {
        if(subTrees_[i][j][k]) delete subTrees_[i][j][k];
    }
    if(remotePatches_) {
        delete remotePatches_;
    }
    if(superPatches_) {
        delete superPatches_;
    }
    
    int flag;
    MPI_Finalized(&flag);

    if(!flag && MPI_COMM_NULL != comm_) {
        MPI_Comm_free(&comm_);
    }
}

template<int D>
void GlobalTree<D>::queryAll(Query<D>& query)
{
    if(2 == D) {
        forall2(i,j) {
            if(subTrees_[i][j][1]) subTrees_[i][j][1]->queryTree(query);
        }
    } else {
        forall3(i,j,k) {
            if(subTrees_[i][j][k]) subTrees_[i][j][k]->queryTree(query);
        }
    }
}

template<int D>
int GlobalTree<D>::getNumSuperpatches()
{
    int num = 0;

    if(2 == D) {
        forall2(i,j) {
            if(subTrees_[i][j][1]) num += (subTrees_[i][j][1]->getNumNodes() - subTrees_[i][j][1]->getNumLeaves());
        }
    } else {
        forall3(i,j,k) {
            if(subTrees_[i][j][k]) num += (subTrees_[i][j][1]->getNumNodes() - subTrees_[i][j][1]->getNumLeaves());
        }
    }
    
    return num;
}

template<int D>
void GlobalTree<D>::constructOnAll(HierarchConstr& recipe)
{
    if(2 == D) {
        forall2(i,j) {
            if(subTrees_[i][j][1]) subTrees_[i][j][1]->constructHierarch(recipe);
        }
    } else {
        forall3(i,j,k) {
            if(subTrees_[i][j][k]) subTrees_[i][j][k]->constructHierarch(recipe);
        }
    }
}

#undef forall2
#undef forall3

}

#endif

