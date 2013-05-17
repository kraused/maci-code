
// vi: tabstop=4:expandtab

#ifndef PULIB_TYPEDEFS_HH_INCLUDED
#define PULIB_TYPEDEFS_HH_INCLUDED 1

#include "polylib/polylib.hh"
#include "treelib/treelib.hh"

namespace pulib
{

// A collection of various types
template<int D>
struct Types
{
    // A rectangular box
    typedef typename treelib::Types<D>::Box     Box;
    // A vector with three components
    typedef typename treelib::Types<D>::double3 double3;
};

}

#endif

