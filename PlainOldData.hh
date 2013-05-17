
// vi: tabstop=4:expandtab

#ifndef MACI_PLAIN_OLD_DATA_HH_INCLUDED
#define MACI_PLAIN_OLD_DATA_HH_INCLUDED 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Base class for all C structures
template<typename Derived>
struct PlainOldData
{

    /// Constructor which ensures that the whole structure is zero'ed
    PlainOldData()
    {
        memset(this, 0x0, sizeof((Derived *)this));
    }

//  /// Copy constructor
//  /// @param other    the other instance
//  PlainOldData(const PlainOldData& other)
//  {
//      memcpy(this, &other, sizeof((Derived *)this);
//  }
//
//  /// Assignment operator
//  /// @param other    the other instance
//  /// @returns        reference to this after assignement
//  PlainOldData& operator=(const PlainOldData& other)
//  {
//      memcpy(this, &other, sizeof((Derived *)this);
//  }
};

#endif /* MACI_PLAIN_OLD_DATA_HH_INCLUDED */

