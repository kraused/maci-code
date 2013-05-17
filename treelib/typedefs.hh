
// vi: tabstop=4:expandtab

#ifndef TREELIB_TYPEDEFS_HH_INCLUDED
#define TREELIB_TYPEDEFS_HH_INCLUDED 1

#include "polylib/polylib.hh"

namespace treelib {

    /// A collection of various types
    template<int D>
    struct Types
    {
        /// A rectangular box
        typedef typename polylib::Box<double,D> Box;
        /// A vector with three components
        typedef double              double3[3];
        /// A flag type for storing bits
        typedef unsigned short          ShortFlag;
    };

}

#endif

