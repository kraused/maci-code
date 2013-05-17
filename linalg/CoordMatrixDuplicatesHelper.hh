
// vi: tabstop=4:expandtab

#ifndef MACI_LINALG_COORD_MATRIX_DUPLICATES_HELPER_HH_INCLUDED
#define MACI_LINALG_COORD_MATRIX_DUPLICATES_HELPER_HH_INCLUDED 1

#include <algorithm>

namespace linalg {

    /// Helper structure for the handling of duplicate coordinate matrix
    /// entries
    struct CoordMatrixDuplicatesHelper
    {

        /// Remove duplicates entries from a range
        /// This is the gnu stl implementation of std::unique
        template<typename ForwardIterator>
        static ForwardIterator remove(ForwardIterator first, ForwardIterator last)
        {
            std::sort(first, last);

            first = std::adjacent_find(first,last);
            if(first == last)
                return last; 
    
            ForwardIterator dest = first;
            ++first;
            while(++first != last) 
            {
                if((*dest) != (*first))
                    *++dest = *first;
            }

                return ++dest;
        }

        /// Sum up duplicate entries
        template<typename ForwardIterator>
        static ForwardIterator sumup(ForwardIterator first, 
                         ForwardIterator last)
        {   
            std::sort(first, last);

            first = std::adjacent_find(first,last);
            if(first == last)
                return last;
        
            ForwardIterator dest   = first;
            ForwardIterator second = first;
        
            while(++first != last) 
            {
                if((*dest) != (*first)) 
                {
                    *++dest  = *first; ++second;
                } 
                else 
                {
                    (*second) += (*first);
                }
            }
        
            return ++dest;
        }

    };
}

#endif

