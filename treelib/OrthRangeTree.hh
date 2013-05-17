
// vi: tabstop=4:expandtab

#ifndef TREELIB_ORTH_RANGE_TREE_HH_INCLUDED
#define TREELIB_ORTH_RANGE_TREE_HH_INCLUDED 1

#include "polylib/polylib.hh"
#include "treelib/typedefs.hh"
#include "treelib/Query.hh"
#include "treelib/HierarchConstr.hh"

namespace treelib
{

// The nodes of an orthogonal range tree
template<int D, typename T>
class Node4OrthRangeTree
{
    typedef typename Types<D>::Box          Box;
    typedef typename Types<D>::double3      double3;
    typedef typename Types<D>::ShortFlag    Flag;

public:
    typedef Node4OrthRangeTree<D,T>         Node;
    
public:
    
    // Construct a new node without caring about the point (can't be a leaf)
    Node4OrthRangeTree(int level, const Box& box, T* data = 0);
    // Construct a new node
    Node4OrthRangeTree(int level, const double* point, const Box& box, bool isLeaf, T* data = 0);
    
    // Insert a new node in the subtree below @this
    void insertInSubtree(const double* pt, T& obj);
    
    // Kill the subtree below @this
    void killSubtree();
    
    // Help the tree implementation to perform a tree query
    void supportTreeQuery(RangeTreeQuery<D,T>& query);
    
    // Help the tree in applying a hierarchical construction recipe
    void supportHierarchConstruction(HierarchConstr<D,T>& recipe);
    
    // Get the point of the node
    inline const double3& getPoint() const
    {
        return point_;
    }
    
    // Get the bounding box of the node
    inline const Box& getBox() const
    {
        return box_;
    }
    
    // Question: Is the head a leaf? If the tree is empty, this is obviously
    //           not the case. If the tree however is empty, i.e. there was no
    //           particle inserted, the head is a leaf but without associated data
    //           In any case, the head node has a special position
    // Convention: The head node is never a leaf!
    
    // Returns true if the node is a leaf
    inline bool isLeaf() const
    {
        return isLeaf_;
    }
    
    // Unset the leaf flag
    inline void unsetLeaf()
    {
        isLeaf_ = false;
    }
    
    // Returns true if the node is the head of the tree
    inline bool isHead() const
    {
        return 1 == (0x1 & flag_);
    }
    
    // Mark the node as being the root node
    inline void setHeadFlag()
    {
        flag_ |= 0x1;
    }
    
    // Returns the number of sons
    int getNumSons() const;
    
protected:
    // Level of the node
    int         level_;
    // Point of the octant
    double3     point_;
    // The associated box
    Box         box_;
    // True if the node is a leaf
    bool        isLeaf_;
    // Pointer to sons or 0
    Node*       sons_[(1 << D)];
    
private:
    // Internal flag
    Flag        flag_;
    // The user data. This member is private because we basically do not care
    // about this member and we do not have any information about it
    T*          data_;
};


// The base class for all data attachable to
// the tree
// S is the derived subclass. It must be (unfortunately)
// passed as an argument. The alternative would be to use
// a generic void pointer but the loss of type checking will
// affect performance
template<int D, typename S>
class OrthRangeTreeAttachableData
{
    typedef typename Types<D>::Box      Box;
    typedef typename Types<D>::double3  double3;
    
public:
    typedef Node4OrthRangeTree<D,S>     Node;
    
public:
    // Create a new attachable data instance
    // The constructor should not know get any
    // arguments because it is intended as an
    // infinite layer
    OrthRangeTreeAttachableData();
    
    // Get the point of the node
    inline const double3& getPoint() const
    {
        return nd_->getPoint();
    }
    
    // Get the bounding box of the node
    inline const Box& getBox() const
    {
        return nd_->getBox();
    }
    
    // Set the node
    inline void setNodeLink(const Node* nd)
    {
        nd_ = nd;
    }
    
private:
    // Pointer to the node
    const Node*     nd_;
};


// An orthogonal range tree; D is the dimension, i.e. 2 or 3;
// T must be derived from OrthRangeTreeAttachableData
// The coupling between trees and attachable is rather weak. The user
// is responsible for the construction and destruction of the data.
// Data and nodes are coupled via pointers.
template<int D, typename T>
class OrthRangeTree
{
    typedef typename Types<D>::double3  double3;
    typedef Node4OrthRangeTree<D,T>     Node;
    
public:
    typedef typename Types<D>::Box      Box;
    
public:
    
    // Construct a new ORT given a bounding box
    OrthRangeTree(const Box& box);
    
    // Destroy the tree and remove all nodes
    ~OrthRangeTree();
    
    // Insert point with attached data
    void insert(const double* pt, T& obj);
    
    // Query the tree
    void queryTree(RangeTreeQuery<D,T>& query) const;
    
    // Apply a hierarchical construction recipe
    void constructHierarch(HierarchConstr<D,T>& recipe);
    
    // Get the bounding box of the tree
    inline const Box& getBoundingBox() const
    {
        return head_->getBox();
    }
    
    // Get the number of nodes
    int getNumNodes();
    
    // Get the number of leaves
    int getNumLeaves();
    
    // Returns true iff the tree is empty
    inline bool isEmpty() const
    {
        return (0 == head_->getNumSons());
    }
    
protected:
    // The head of the node
    Node*   head_;
};


template<int D, typename T>
Node4OrthRangeTree<D,T>::Node4OrthRangeTree(int level, const Box& box, T* data)
: level_(level), box_(box), isLeaf_(false), flag_(0), data_(data)
{
    std::fill(point_, point_+3, 0.0);
    memset(sons_, 0x0, (1 << D)*sizeof(Node*));

    if(data_) {
        data_->setNodeLink(this);
    }
}
    
template<int D, typename T>
Node4OrthRangeTree<D,T>::Node4OrthRangeTree(int level, const double* point, const Box& box, bool isLeaf, T* data)
: level_(level), box_(box), isLeaf_(isLeaf), flag_(0), data_(data)
{
    std::copy(point, point+3, point_);
    memset(sons_, 0x0, (1 << D)*sizeof(Node*));
    
    if(data_) {
        data_->setNodeLink(this);
    }
}

template<int D, typename T>
void Node4OrthRangeTree<D,T>::insertInSubtree(const double* pt, T& obj)
{
    double3 lower, upper;
    int     b = 0;

    ASSERT(box_.isInside(pt,1e-5*box_.getVolume()));

    for(int d = D-1; d >= 0; --d) {
        if(pt[d] < 0.5*(box_.getUpper()[d] + box_.getLower()[d])) {
            b *= 2;
            
            lower[d] = box_.getLower()[d];
            upper[d] = 0.5*(box_.getLower()[d] + box_.getUpper()[d]);
        }
        else {
            b *= 2;
            b += 1;
            
            lower[d] = 0.5*(box_.getLower()[d] + box_.getUpper()[d]);
            upper[d] = box_.getUpper()[d];
        }
    }
    
    if(sons_[b]) {
        sons_[b]->insertInSubtree(pt, obj);
    } else {
        sons_[b] = new Node(level_+1, pt, Box(lower,upper), true, &obj);
        
        if(isLeaf() and !isHead()) {
            unsetLeaf();            
            insertInSubtree(point_, *data_);
            // There is no data anymore
            data_ = 0;
        }
    }
}

template<int D, typename T>
void Node4OrthRangeTree<D,T>::killSubtree()
{
    if(isLeaf()) {
        return;
    } else {
        killSubtree();
        
        for(int i = 0; i < (1 << D); ++i) {
            if(sons_[i]) {
                delete sons_[i];
            }
        }
        
    }
}

template<int D, typename T>
void Node4OrthRangeTree<D,T>::supportTreeQuery(RangeTreeQuery<D,T>& query)
{
    if(false == query.matches(point_, box_, *data_)) {
        return;
    }
    
    if(false == isLeaf()) {
        for(int i = 0; i < (1 << D); ++i) {
            if(sons_[i]) {
                sons_[i]->supportTreeQuery(query);
            }
        }
        
        // Only leafs are processed
        return;
    }
    
    // If the root box is a leaf, the tree is empty and everything is silly
    if(isHead()) {
        return;
    }
    
    query.process(point_, box_, *data_);
}
    
template<int D, typename T>
void Node4OrthRangeTree<D,T>::supportHierarchConstruction(HierarchConstr<D,T>& recipe)
{
    const T* sonobj[(1 << D)];

    if(false == isLeaf()) {     
        for(int i = 0; i < (1 << D); ++i) {
            if(sons_[i]) {
                sons_[i]->supportHierarchConstruction(recipe);
                
                sonobj[i] = sons_[i]->data_;
            } else {
                sonobj[i] = 0;
            }
        }
    } /* else {
        memset(sonobj, 0x0, (1<<D)*sizeof(T*));
    } */
    
    ASSERT((isLeaf() && data_) || (0 == data_));

    data_ = recipe.construct(point_, box_, data_, sonobj);
}

template<int D, typename T>
int Node4OrthRangeTree<D,T>::getNumSons() const
{
    int num = 0;    
    for(int i = 0; i < (1 << D); ++i) {
        if(sons_[i]) ++num;
    }   
    return num;
}


template<int D, typename S>
OrthRangeTreeAttachableData<D,S>::OrthRangeTreeAttachableData()
: nd_(0)
{
}

                        
template<int D, typename T>
OrthRangeTree<D,T>::OrthRangeTree(const OrthRangeTree<D,T>::Box& box)
: head_(new Node(0, box))
{
    head_->setHeadFlag();
}

template<int D, typename T>
OrthRangeTree<D,T>::~OrthRangeTree()
{
}
    
template<int D, typename T>
void OrthRangeTree<D,T>::insert(const double* pt, T& obj)
{
    head_->insertInSubtree(pt, obj);
}

template<int D, typename T>
void OrthRangeTree<D,T>::queryTree(RangeTreeQuery<D,T>& query) const
{
    if(!isEmpty()) {
        head_->supportTreeQuery(query);
    }
}

template<int D, typename T>
void OrthRangeTree<D,T>::constructHierarch(HierarchConstr<D,T>& recipe)
{
    if(!isEmpty()) {
        head_->supportHierarchConstruction(recipe);
    }
}

// Query to obtain nodal statistics. It is not optimal to require
// a query to get these informations about it is the most straigthforward
// implementation
template<int D, typename T>
class OrthRangeTreeStatisticQuery : public RangeTreeQuery<D,T>
{
    
public:
    typedef RangeTreeQuery<D,T> Base;

    OrthRangeTreeStatisticQuery()
    : numNodes_(0), numLeaves_(0)
    {
    }
    
    bool matches(const double* pt, const typename Base::Box& B, const T& obj)
    {
        ++numNodes_;
        return true;
    }
    
    void process(const double* pt, const typename Base::Box& B, const T& obj)
    {
        ++numLeaves_;
    }   
    
    // Get the number of nodes
    inline int getNumNodes() const
    {
        return numNodes_;
    }
    
    // Get the number of leaves
    inline int getNumLeaves() const
    {
        return numLeaves_;
    }
    
private:
    // Number of nodes
    int     numNodes_;
    // Number of leaves
    int     numLeaves_;
};

template<int D, typename T>
int OrthRangeTree<D,T>::getNumNodes()
{
    OrthRangeTreeStatisticQuery<D,T> query;
    queryTree(query);
    return query.getNumNodes();
}

template<int D, typename T>
int OrthRangeTree<D,T>::getNumLeaves()
{
    OrthRangeTreeStatisticQuery<D,T> query;
    queryTree(query);
    return query.getNumLeaves();
}

}

#endif

