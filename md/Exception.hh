
// vi: tabstop=4:expandtab

#ifndef MD_EXCEPTION_HH_INCLUDE
#define MD_EXCEPTION_HH_INCLUDE 1

#include <exception>

namespace md {
    namespace interface {
    
        // Indicates that the function is not supported, e.g. because
        // it doesn't make sense for this implementation
        class FunctionNotSupportedException : public std::exception
        {
        
        };
    
    }
}

#endif

