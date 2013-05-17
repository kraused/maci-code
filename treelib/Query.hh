
// vi: tabstop=4:expandtab

#ifndef TREELIB_QUERY_HH_INCLUDED
#define TREELIB_QUERY_HH_INCLUDED 1

#include "polylib/polylib.hh"
#include "treelib/typedefs.hh"

namespace treelib {

    /// Abstract base class for all queries 
    template<int D, typename T>
    class RangeTreeQuery
    {

    public:
        typedef typename Types<D>::Box  Box;
    
    public:
        // Function used to determine if the current octant is a candidate for a
        // match. If so either @process is called if the octant is locally stored
        // or the candidates are queued and sended to the remote source processor
        // of the query.
        virtual bool matches(const double* pt, const Box& B, const T& obj) = 0;
    
        // Function used to process a query result.
        // Note: It is guaranteed that matches() returns true!
        virtual void process(const double* pt, const Box& B, const T& obj) = 0;
    
        virtual ~RangeTreeQuery()
        {
        }
    };

}

#endif

