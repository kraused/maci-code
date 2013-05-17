
// vi: tabstop=4:expandtab

#ifndef TREELIB_HIERARCH_CONSTR_HH_INCLUDED
#define TREELIB_HIERARCH_CONSTR_HH_INCLUDED 1

#include "polylib/polylib.hh"
#include "treelib/typedefs.hh"

namespace treelib
{

template<int D, typename T>
class RangeTree;


// Abstract base class for all hiearchical construction
// procedure
template<int D, typename T>
class HierarchConstr
{

public:
    typedef typename Types<D>::Box  Box;
    
public:
    // Function used to construct for a given tree object. The return value
    // is the new associated data. The reassociation is only done if the
    // data object of the node is 0.
    // Note: Again you are responsible for destroying the allocated memory
    //       by yourself. See @RangeTree.hh for a description of the loose
    //       data coupling concept.
    // Warning: Note that a instance of @T is associated to *every* node of the
    //          tree. Therefor the memory footprint of @T should be as small as possible
    // @data is 0 iff the node is not a leaf
    virtual T* construct(const double* pt, const Box& B, T* obj, const T* sonobj[(1 << D)]) = 0;

    // Required for smooth compilation ...
    virtual ~HierarchConstr()
    {
    }
};

}

#endif

