
// vi: tabstop=4:expandtab

#include "puTree.hh"

#include <list>
#include <math.h>

namespace pulib
{

// Get global maximal patch sidelength
template<int D>
void globalMaxPatchSidelength(const Basis<D>& S, MPI_Comm comm, typename GlobalTree<D>::double3 hJ)
{
    S.getMaxPatchSidelength(hJ);
    MPI_Allreduce(MPI_IN_PLACE, hJ, 3, MPI_DOUBLE, MPI_MAX, comm);
}

template
void globalMaxPatchSidelength<2>(const Basis<2>& S, MPI_Comm comm, GlobalTree<2>::double3 hJ);

template
void globalMaxPatchSidelength<3>(const Basis<3>& S, MPI_Comm comm, GlobalTree<3>::double3 hJ);

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

// Manager for the particle exchange
template<int D, typename T>
class ExchangeManager
{

public:
    // For convenience ...
    typedef int int3[3];

    ExchangeManager(MPI_Comm comm, const int nbranks[])
    : comm_(comm), recvBuf_(0)
    {
        MPI_Comm_size(comm, &numprocs_);
        MPI_Comm_rank(comm, &myrank_);

        int l = 0;
        if(2 == D) {
            forall2(i,j) {
                nbRanks_[i][j][1] = nbranks[l++];
            }
        } else {
            forall3(i,j,k) {
                nbRanks_[i][j][k] = nbranks[l++];
            }
        }

        numItem2Send_ = new int[numprocs_]();
        numItem2Recv_ = new int[numprocs_]();
        
        MPI_Type_contiguous(sizeof(T), MPI_BYTE, &typeOfT_);
        MPI_Type_commit(&typeOfT_);
    }
    
    ~ExchangeManager()
    {
        MPI_Type_free(&typeOfT_);
    
        delete[] numItem2Send_;
        delete[] numItem2Recv_;
        
        if(recvBuf_) delete[] recvBuf_;
    }
        
    // Schedule a instance for send to the target
    // with displ @dx, @dy, @dz from this processing
    // element
    inline void scheduleForSend(const T& obj, const int* d)
    {
        scheduleForSend(d[0], d[1], d[2]);
    }

    void scheduleForSend(const T& obj, int dx, int dy, int dz)
    {
        if(0 == dx && 0 == dy && 0 == dz) {
            return;
        }
        
        ASSERT(0 == dx || -1 == dx || 1 == dx);
        ASSERT(0 == dy || -1 == dy || 1 == dy);
        ASSERT(0 == dz || -1 == dz || 1 == dz);
        
        sendBuf_[dx+1][dy+1][dz+1].insert(obj);
    }

    // Exchange items
    void exchangeItems()
    {
        compNumSendItems();
        commNumRecvItems();
        exchangeBuffers();
    }
    
    // Iterator over iterated
    class RecvItemIterator
    {
    
    public:
        // Construct a new iterator with current position = @idx
        RecvItemIterator(const ExchangeManager<D,T>& manager, int idx)
        : manager_(manager), idx_(idx)
        {
        }
        
        // Copy constructor
        RecvItemIterator(const RecvItemIterator& obj)
        : manager_(obj.manager_), idx_(obj.idx_)
        {
        }
        
        // Compare iterator
        bool operator<(const RecvItemIterator& obj) const
        {
            return idx_ < obj.idx_;
        }
        
        bool operator!=(const RecvItemIterator& obj) const
        {
            return idx_ != obj.idx_;
        }
        
        
        // Go to next item
        RecvItemIterator& operator++()
        {
            ++idx_;
            return *this;
        }
                
        // Access to data
        const T& operator*() const
        {
            return manager_.recvBuf_[idx_];
        }
    
        // Get the rank of the source processor
        int getSrc() const
        {
            return manager_.getSrcOfItem(idx_);
        }
        
        // Get the relative coordinates of the source
        // processor
        void getSrc(int ijk[3]) const
        {
            manager_.getSrcOfItem(idx_, ijk);
        }
    
    private:
        // The manager instance
        const ExchangeManager<D,T>& manager_;
        // Index (current position)
        int             idx_;
    };
    
    friend class RecvItemIterator;
    
    RecvItemIterator begin() const
    {
        return RecvItemIterator(*this,0);
    }
    
    RecvItemIterator end() const
    {
        return RecvItemIterator(*this,getRecvbufferSize());
    }
    
private:    
    // Returns the number of items to be send
    int getSendbufferSize() const
    {
        int num = 0;

        if(2 == D) {
            forall2(i,j) {
                num += sendBuf_[i][j][1].size();
            }
        } else {
            forall3(i,j,k) {
                num += sendBuf_[i][j][k].size();
            }
        }
        
        return num;
    }
    
public: 
    // Returns the number of items received
    int getRecvbufferSize() const
    {
        int num = 0;
        
        for(int i = 0; i < numprocs_; ++i) {
            num += numItem2Recv_[i];
        }

        return num;
    }
    
private:
    // Concat all the sendbuffers into one hugh buffer
    T* concatSendBuffers() const
    {
        int size;
        if(0 == (size = getSendbufferSize())) {
            return 0;
        }
        
        T*  sendbuf = new T[size];
        int ii = 0;
        
        for(int p = 0; p < numprocs_; ++p) {
            if(2 == D) {
                forall2(i,j) {
                    if(p == nbRanks_[i][j][1]) {
                        ii += sendBuf_[i][j][1].copy(&sendbuf[ii]);
                    }
                }
            } else {
                forall3(i,j,k) {
                    if(p == nbRanks_[i][j][k]) {
                        ii += sendBuf_[i][j][k].copy(&sendbuf[ii]);
                    }
                }
            }   
        }
                
        return sendbuf;
    }
    
    // Compute the number of items to send
    void compNumSendItems()
    {
        int rank;
    
        // Attention: It is important to sum the entries up (+=)
        //            instead of assigning them because multiple
        //            displacements might correspond to the same
        //            processor in periodic meshes
        if(2 == D) {
            forall2(i,j) {
                if(MPI_PROC_NULL != (rank = nbRanks_[i][j][1])) {
                    numItem2Send_[rank] += sendBuf_[i][j][1].size();
                }
            }       
        } else {
            forall3(i,j,k) {
                if(MPI_PROC_NULL != (rank = nbRanks_[i][j][k])) {
                    numItem2Send_[rank] += sendBuf_[i][j][k].size();
                }
            }
        }
    }
    
    // Give an array containing nonnegative integers (sizes) this
    // function computes the displacements of the individual regions
    static int* getDisplacements(int* array, int N) {
        int* displ = new int[N]();
        
        memset(displ, 0x0, N*sizeof(int));
        for(int i = 1; i < N; ++i) {
            displ[i] = displ[i-1] + array[i-1];
        }
        
        return displ;
    }
    
    // Compute the number of items to receive
    // Warning: It is very important that @getNumSendItems
    //          is called *before* @getNumRecvItems because
    //          otherwise the number of items to receive
    //          is always 0
    inline void commNumRecvItems()
    {       
        MPI_Alltoall(numItem2Send_, 1, MPI_INT, numItem2Recv_, 1, MPI_INT, comm_);
    }
    
    // Allocate the receive buffer
    // Warning: Call only after @getNumRecvItems
    void allocRecvBuffer()
    {
        int totalNumItems2Recv = 0;
        for(int i = 0; i < numprocs_; ++i) {
            totalNumItems2Recv += numItem2Recv_[i];
        }
        
        if(0 < totalNumItems2Recv) {
            recvBuf_ = new T[totalNumItems2Recv];
        } else {
            recvBuf_ = 0;
        }
    }
    
    // Exchange the buffers
    // The function allocates the receive buffer!
    void exchangeBuffers()
    {
        T*   sendbuf = concatSendBuffers();
        int* sdispls = getDisplacements(numItem2Send_, numprocs_);
        int* rdispls = getDisplacements(numItem2Recv_, numprocs_);
        
        allocRecvBuffer();
        
#ifndef NDEBUG
        if(!sendbuf) {
            for(int i = 0; i < numprocs_; ++i) {
                ASSERT(0 == numItem2Send_[i]);
            }
        }
        if(!recvBuf_) {
            for(int i = 0; i < numprocs_; ++i) {
                ASSERT(0 == numItem2Recv_[i]);
            }
        }
#endif
        
        MPI_Alltoallv(sendbuf, numItem2Send_, sdispls, typeOfT_, recvBuf_, numItem2Recv_, rdispls, typeOfT_, comm_);
                
        delete[] sendbuf;
        delete[] sdispls;
        delete[] rdispls;
    }
    
    // Get the source rank of a specific item
    int getSrcOfItem(int i) const
    {
        int* displs = getDisplacements(numItem2Recv_, numprocs_);
                
        int k = 0;
        for(; k < numprocs_; ++k) {
            if(i >= displs[k] && (k == numprocs_-1 || i < displs[k+1])) {
                break;
            }
        }
        
        delete[] displs;
        
        if(k == numprocs_) {
            k = -1;
        }
                
        return k;
    }
    
    // Get the relative coordinates of the source rank of a specific item
    void getSrcOfItem(int i, int ijk[3]) const
    {
        convertRankToRelCoords(getSrcOfItem(i), ijk);
    }   
    
    // Convert a rank to relative coordinates
    void convertRankToRelCoords(int rank, int ijk[3]) const
    {   
        // If this turns out to be a performance problem, the results
        // can easily be cached
        if(2 == D) {
            forall2(i,j) {
                if(nbRanks_[i][j][1] == rank) {
                    ijk[0] = i-1;
                    ijk[1] = j-1;
                    ijk[2] = 0;
                    return;
                }
            }
        } else {
            forall3(i,j,k) {
                if(nbRanks_[i][j][k] == rank) {
                    ijk[0] = i-1;
                    ijk[1] = j-1;
                    ijk[2] = k-1;
                    return;
                }           
            }
        }

        /* should not happen */
        ASSERT(1 == 0);
    }
    
    // A send buffer. This is a dynamical growing
    // array
    class SendBuffer
    {
    
    public:
        SendBuffer()
        : mem_(0), length_(0), cnt_(0)
        {
        }
        
        ~SendBuffer()
        {
            if(mem_) {
                free(mem_);
            }
        }
        
        // Insert a new point
        void insert(const T& pt) {
            if(length_ - cnt_ < 2) {
                mem_     = (T* )realloc(mem_, (length_ + STEP)*sizeof(T));
                length_ += STEP;
            }
            
            memcpy(mem_[cnt_++],pt,sizeof(T));
        }
        
        // Return the size
        inline int size() const
        {
            return cnt_;
        }
        
        // Copy the whole buffer into a new one; Returns
        // the number of items copied
        int copy(T* buf) const
        {
            memcpy(buf, mem_, cnt_*sizeof(T));
            return cnt_;
        }
            
    private:
        // The memory
        T*  mem_;
        // The length of @mem
        int length_;
        // The first free entry
        int cnt_;
        
        // The amount of mem to add when we run out of memory
        // You get the actual numbe of bytes when multiplying
        // by sizeof(T)
        static const int STEP = 8;
    };
    
private:
    // The communicator
    MPI_Comm        comm_;
    // Size of the group of @comm_
    int         numprocs_;
    // My rank
    int         myrank_;
    // Ranks of the neighbors
    int         nbRanks_[3][3][3];
    // Send buffers
    // Unfortunately the storage format makes this one inappropriate
    // for MP; we have the swap into a large contiguous array
    SendBuffer      sendBuf_[3][3][3];
    // Number of items to send to each other processor
    // Computed in @getNumItem2Send
    int*            numItem2Send_;
    // The receive buffer
    // Allocated in @allocRecvBuffer
    T*          recvBuf_;
    // Number of items to receive from each other processor
    // Computed in @getNumItem2Recv
    int*            numItem2Recv_;
    // The MPI Datatype of @T
    MPI_Datatype        typeOfT_;
};

// Ask for the particles near the boundary
template<int D>
class FindHaloParticlesQuery : public Query<D>
{

public:
    typedef double                      double3[3];
    typedef typename Query<D>::Box      Box;
    typedef short                       short3[3];
    typedef double                      double6[6];
    
public:
    FindHaloParticlesQuery(const Box& box, const double3 hJ, 
            ExchangeManager<D,double6>& manager)
    : box_(box), manager_(manager)
    {
        for(int d = 0; d < D; ++d) {
            hJ_[d] = hJ[d];
        }
    }
        
    bool matches(const double* pt, const Box& B, const Patch<D>& obj)
    {
        return boxIntersectsHalo(B);
    }

    void process(const double* pt, const Box& B, const Patch<D>& obj)
    {
        if(false == pointIsInHalo(pt)) {
            return;
        }
        
        double6 pkt;
        
        for(int d = 0; d < D; ++d) {
            pkt[  d] = pt[d];
            pkt[3+d] = obj.getUpper()[d] - obj.getLower()[d];
        }
                        
        scheduleForSend(pkt);
    }
        
private:
    // Check if point is near the boundary in direction @dir
    // If @c is 'u' the upper boundary is chosen, if @c is 'd'
    // the lower one is compared with
    inline bool pointIsNearBoundary(const double* pt, int dir, char c) const
    {
        switch(c) {
        case 'u':
            return (fabs(pt[dir] - box_.getUpper()[dir]) < hJ_[dir]);
        case 'd':
            return (fabs(pt[dir] - box_.getLower()[dir]) < hJ_[dir]);
        }
        
        return false;
    }

    // Returns true if the point is in the halo
    bool pointIsInHalo(const double* pt) const
    {
        bool b = false;
        for(int d = 0; d < D; ++d) {
            b |= (pointIsNearBoundary(pt, d, 'u') || pointIsNearBoundary(pt, d, 'd'));
        }
        
        return b;
    }
    
    // Returns true if the box intersects the halo
    bool boxIntersectsHalo(const Box& B) const
    {
        bool    b = false;
        double3 c;
        for(int i = 0; i < (1 << D); ++i) {
            for(int d = 0; d < D; ++d) {
                c[d] = B.getCorner(i)[d];
            }
            
            b |= pointIsInHalo(c);
        }
        
        return b;
    }
    
    // Initialize list of keys
    void initializeKeyList(int* keys, int N)
    {
        for(int i = 0; i < N; ++i) {
            keys[i] = -1;
        }
    }
    
    // Check if a key for a direction is alreay in the
    // list and insert it otherwise
    bool checkKeyOrInsert(int* keys, int N, int key)
    {
        for(int i = 0; i < N; ++i) {
            if(keys[i] == key) {
                return false;
            }
        }
        
        keys[N] = key;
        return true;
    }
                
    // Schedule a packet for send; The function computes
    // the offset from this processor and calls the
    // corresponding function of the exchange manager
    // Note: It is important to notice that one particle can
    //       be send to multiple target processors!
    void scheduleForSend(const double6& pkt)
    {
        int  dir[3] = { 0, 0, 0 };
        int  keys[(1 << D)-1];
        
        for(int d = 0; d < D; ++d) {
            if(pointIsNearBoundary(pkt, d, 'u')) {
                dir[d] = +1;
            }
            if(pointIsNearBoundary(pkt, d, 'd')) {
                dir[d] = -1;
            }
        }
        
        // Combinations of directions for @D = 2
        int combs2[3][2] = { {1,0}, {0,1}, {1,1} };
        // Combinations of directions for @D = 3
        int combs3[7][3] = { {1,0,0}, {0,1,0}, {0,0,1},
                     {1,1,0}, {0,1,1}, {1,0,1}, {1,1,1} };
        
        initializeKeyList(keys,(1 << D)-1);
        
        if(2 == D) {
            for(int i = 0; i < (1 << D)-1; ++i) {
                int key = 3*(dir[1]*combs2[i][1] + 1) + \
                        (dir[0]*combs2[i][0] + 1);
            
                if(checkKeyOrInsert(keys, i, key)) {
                    manager_.scheduleForSend(pkt, dir[0]*combs2[i][0],
                                      dir[1]*combs2[i][1], 0);
                }
            }
        } else {
            for(int i = 0; i < (1 << D)-1; ++i) {
                int key = 9*(dir[2]*combs3[i][2] + 1) + \
                          3*(dir[1]*combs3[i][1] + 1) + \
                        (dir[0]*combs3[i][0] + 1);
            
                if(checkKeyOrInsert(keys, i, key)) {
                    manager_.scheduleForSend(pkt, dir[0]*combs3[i][0],
                                      dir[1]*combs3[i][1],
                                      dir[2]*combs3[i][2]);
                }
            }   
        }
    }
    
private:
    // The local box
    const Box&          box_;
    // The halo size; Can't be const because I want it to be owned by the
    // class and there is no "copy constructor" for arrays
    double3             hJ_;
    // The exchange manager
    ExchangeManager<D,double6>& manager_;
};

// Move box in dimension @d in direction @dir and make sure
// that the resulting box is not larger than @hJ
// The amount of translation is equal to the box size itself
template<int D>
inline void moveAndShrinkBox(double* lower, double* upper, const double* hJ, 
        int d, int dir)
{
    switch(dir) {
    case +1:
        lower[d] = upper[d];
        upper[d] = lower[d] + hJ[d];
        break;
    case -1:
        upper[d] = lower[d];
        lower[d] = upper[d] - hJ[d];
        break;
    default:
        return;
    }
}

// Copy the lower and upper corner coordinates
template<int D>
inline void copyLowerAndUpper(const typename GlobalTree<D>::Box& box, 
            double* lower, double* upper)
{
    for(int d = 0; d < D; ++d) {
        lower[d] = box.getLower()[d];
        upper[d] = box.getUpper()[d];
    }
}

// Build the surrounding trees; The function gets the bounding
// box of the local tree and the halo thickness. The bounding
// boxes of the remote trees are then only as large as the 
// halo
template<int D>
void buildRemoteTrees(Tree<D>* tree[3][3][3], 
        const typename GlobalTree<D>::Box& box, const double* hJ)
{
    typename GlobalTree<2>::double3 lower, upper;
    
    if(2 == D) {
        forall2(i,j) {
            if(1 == i && 1 == j) {
                continue;
            }
            
            copyLowerAndUpper<D>(box, lower, upper);
            
            moveAndShrinkBox<D>(lower, upper, hJ, 0, i-1);
            moveAndShrinkBox<D>(lower, upper, hJ, 1, j-1);
            
            tree[i][j][1] = new Tree<D>(typename GlobalTree<D>::Box(lower,upper));
        }
    } else {
        forall3(i,j,k) {
            if(1 == i && 1 == j && 1 == k) {
                continue;
            }
        
            copyLowerAndUpper<D>(box, lower, upper);
                    
            moveAndShrinkBox<D>(lower, upper, hJ, 0, i-1);
            moveAndShrinkBox<D>(lower, upper, hJ, 1, j-1);
            moveAndShrinkBox<D>(lower, upper, hJ, 2, k-1);
            
            tree[i][j][k] = new Tree<D>(typename GlobalTree<D>::Box(lower,upper));      
        }
    }
}

// Create a new patch with point @pt and sidelengths @hJ, add it to the arena
// and return a reference to it
template<int D>
Patch<D>& getNewPatchFromArena(treelib::Arena* arena, const double* pt, const double* hJ)
{
    typename GlobalTree<D>::double3 lower, upper;
    
    for(int d = 0; d < D; ++d) {
        lower[d] = pt[d] - 0.5*hJ[d];
        upper[d] = pt[d] + 0.5*hJ[d];
    }
    
    Patch<D>* mem = (Patch<D>* )arena->alloc(sizeof(Patch<D>));
    new(mem) Patch<D>(lower, upper);
    
    return *mem;
}

// Get the correct tree for the remote item
template<int D>
Tree<D>* getRemoteTreeForRecvItem(Tree<D>* tree[3][3][3], 
        const typename ExchangeManager<D,double[6]>::RecvItemIterator& it)
{
    int ijk[] = { 0, 0, 0 };
    it.getSrc(ijk);
    
    ASSERT(ijk[0] >= -1 && ijk[0] <= 1);
    ASSERT(ijk[1] >= -1 && ijk[1] <= 1);
    ASSERT(ijk[2] >= -1 && ijk[2] <= 1);
        
    return tree[ijk[0]+1][ijk[1]+1][ijk[2]+1];
}

// Insert remote particles in the tree
template<int D>
void insertRemoteParticles(Tree<D>* tree[3][3][3], 
    const typename GlobalTree<D>::Box& box, const double* hJ, 
    MPI_Comm comm, const int nbranks[], treelib::Arena** arena)
{
    // We need to send particles and their hK. Therefore we use double[6]
    ExchangeManager<D,double[6]>    manager(comm, nbranks);
    FindHaloParticlesQuery<D>   query(box, hJ, manager);

    buildRemoteTrees(tree, box, hJ);
    tree[1][1][1]->queryTree(query);

    manager.exchangeItems();
    // Allocate arena for remote patches
    (*arena) = new treelib::Arena(manager.getRecvbufferSize()*sizeof(Patch<D>));
    for(typename ExchangeManager<D,double[6]>::RecvItemIterator it = manager.begin(); it != manager.end(); ++it) {
            // In debug mode it checks if the particle is inside the bounding box
            getRemoteTreeForRecvItem(tree, it)->insert(*it, getNewPatchFromArena<D>(*arena, &(*it)[0], &(*it)[3]));     
    }
}

template
void insertRemoteParticles<2>(Tree<2>* tree[3][3][3], const GlobalTree<2>::Box& box,
        const double* hJ, MPI_Comm comm, const int nbranks[], treelib::Arena** arena);

template
void insertRemoteParticles<3>(Tree<3>* tree[3][3][3], const GlobalTree<3>::Box& box,
        const double* hJ, MPI_Comm comm, const int nbranks[], treelib::Arena** arena);


// Hierarchical Construction recipe for superpatches
template<int D>
class SuperpatchConstruction : public treelib::HierarchConstr<D,typename Tree<D>::AssocData>
{

public:
    typedef typename Tree<D>::AssocData             Data;
    typedef treelib::HierarchConstr<D,typename Tree<D>::AssocData>  Base;
    typedef double                          double3[3];
    
    SuperpatchConstruction(treelib::Arena* arena)
    : arena_(arena)
    {
    }
    
    Data* construct(const double* pt, const typename Base::Box& B, Data* obj, const Data* sonobj[(1 << D)])
    {
        double3 lower, upper;
    
        lower[0] = lower[1] = lower[2] = 0.0;
        upper[0] = upper[1] = upper[2] = 0.0;

        if(0 == obj) {
#ifndef NDEBUG
            {
                int i;
                for(i = 0; i < (1 << D); ++i) {
                    if(sonobj[i]) break;
                }
                ASSERT(i < (1 << D));
            }
#endif
    
            constructCoveringBox(sonobj, lower, upper);
            return getPatchFromArena(lower, upper);
        } else {
            return obj;
        }
    }

private:    
    // Construct a box which is larger than the other patches
    inline void constructCoveringBox(const Data* sonobj[(1 << D)], double* lower, double* upper)
    {
        int i;
    
        // Set the first value for comparison
        for(i = 0; i < (1 << D); ++i) {
            if(sonobj[i]) {
                for(int d = 0; d < D; ++d) {
                    lower[d] = sonobj[i]->getLower()[d];
                    upper[d] = sonobj[i]->getUpper()[d];
                }
                
                break;
            }
        }
        
        // Create the largest box
        for(++i; i < (1 << D); ++i) {
            if(sonobj[i]) {
                for(int d = 0; d < D; ++d) {
                    lower[d] = std::min(lower[d], sonobj[i]->getLower()[d]);
                    upper[d] = std::max(upper[d], sonobj[i]->getUpper()[d]);
                }
            }           
        }       
    }
    
    // Get a patch from the arena
    Data* getPatchFromArena(const double lower[], const double upper[])
    {
        Data* mem = (Data* )arena_->alloc(sizeof(Data));
        new(mem) Data(lower, upper);

        return mem;
    }
    
private:
    // The arena for the superpatches
    treelib::Arena* arena_;
};

// Construct the superpatches for intermediate tree nodes
template<int D>
treelib::Arena* buildSuperPatches(GlobalTree<D>* T)
{
    treelib::Arena* arena = new treelib::Arena(T->getNumSuperpatches()*sizeof(Patch<D>));

    SuperpatchConstruction<D> recipe(arena);
    T->constructOnAll(recipe);
    
    return arena;
}

template
treelib::Arena* buildSuperPatches<2>(GlobalTree<2>* T);

template
treelib::Arena* buildSuperPatches<3>(GlobalTree<3>* T);


// Query to find overlapping patches; Data is copied in the
// destructor
template<int D>
class FindOvPatchesQuery : public Query<D>
{

private:
    typedef typename std::list<const Patch<D>*>         List;
    typedef typename std::list<const Patch<D>*>::const_iterator ListIterator;

public:
    typedef typename treelib::OrthRangeTree<D,Patch<D>/**/>::Box    Box;

    FindOvPatchesQuery(BasisFunction<D>* phiJ)
    : phiJ_(phiJ)
    {
    }
    
    ~FindOvPatchesQuery()
    {
        int ii = 0;

        phiJ_->setNumOvPatches(neighbors_.size());
        for(ListIterator it = neighbors_.begin(); it != neighbors_.end(); ++it) {           
            phiJ_->setOvPatch(ii++, *it);
        }       
    }
        
    bool matches(const double* pt, const Box& B, const Patch<D>& obj)
    {
        return phiJ_->hasIntersection(obj,1e-5*std::min(obj.getVolume(),phiJ_->getVolume()));
    }

    void process(const double* pt, const Box& B, const Patch<D>& obj)
    {       
        neighbors_.push_back(&obj);
    }
        
private:
    // The patch we are looking at
    BasisFunction<D>*   phiJ_;
    // List of neighbors
    List            neighbors_;
};

// Construct the list of all overlapping patches for
// all local patches
template<int D>
void queryConnectivity(GlobalTree<D>* T, Basis<D>& S)
{
    for(typename Basis<D>::Iterator it = S.begin(); it != S.end(); ++it) {
        FindOvPatchesQuery<D> query(it);
        T->queryAll(query);
    }
}

template
void queryConnectivity<2>(GlobalTree<2>* T, Basis<2>& S);

template
void queryConnectivity<3>(GlobalTree<3>* T, Basis<3>& S);

}

