
// vi: tabstop=4:expandtab

#ifndef PULIB_PATCH_HH_INCLUDED
#define PULIB_PATCH_HH_INCLUDED 1

#include "pulib/typedefs.hh"

namespace pulib
{

// The patch associated to a tree node. This is a 
// very small class in order to minimize the memory
// footprint. This is necessary since the class is
// asscociated to all tree nodes
template<int D>
class Patch : public Types<D>::Box, public treelib::OrthRangeTreeAttachableData<D,Patch<D>/**/>
{

public:
    typedef typename Types<D>::Box                  Box;
    typedef treelib::OrthRangeTreeAttachableData<D,Patch<D>/**/>    Data;
    
public:
    // Create a normalized patch; This one can be transformed
    // lateron
    // @id is the id of the patch
    Patch(int id);
    // Create a new patch given lower and upper bounds
    Patch(const double lower[], const double upper[]);
    
    // Set the id
    inline void setId(int id)
    {
        myId_ = id;
    }
    
    // Get the id
    inline int getId() const
    {
        return myId_;
    }
    
private:
    // Id of the patch
    int     myId_;
};


template<int D>
Patch<D>::Patch(int id)
: myId_(id)
{
}

template<int D>
Patch<D>::Patch(const double lower[], const double upper[])
: Box(lower, upper)
{
}

}

#endif

