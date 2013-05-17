
// vi: tabstop=4:expandtab

#ifndef MACI_INT64_HH_INCLUDED
#define MACI_INT64_HH_INCLUDED 1

#ifdef __GNUC__
#include <stdint.h>
typedef int64_t int64;
#endif

/// Provides a collection of utility functions to handle int64 types
struct Int64
{

    /// Concat two int32 numbers to one int64
    /// @param i    higher order bits
    /// @param j    lower order bits
    static inline int64 concat(int i, int j)
    {
        return (((int64)i) << 32) | ((int64)j);
    }

    /// Return the higher order bits
    /// @param i    the int64
    static inline int hi(int64 i)
    {
        return (int)(((unsigned int64)i >> 32));
    }

    /// Return the lower orer bits
    /// @param i    the int64
    static inline int lo(int64 i)
    {
        return (int)i;
    }
};

#endif


