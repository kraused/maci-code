
// vi: tabstop=4:expandtab

#ifndef MACI_UTILS_STRPINTF_HH_INCLUDED
#define MACI_UTILS_STRPINTF_HH_INCLUDED 1

#include <string>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

namespace utils 
{

    /// vsprintf version for std::string classes
    /// @param[in]  fmt     the format string (like for vsprintf)
    /// @param[in]  args    the arguments
    std::string strvprintf(const char *fmt, va_list args);

    /// sprintf version for std::string classes
    std::string strprintf(const char *fmt, ...);

    /// Create a string representation of an array
    /// @param[in]  fmt     the format string for each array element
    template<typename Tp>
    std::string list2str(const char* fmt, Tp* start, Tp* end)
    {
        /// Prepare the format string
        std::string f = std::string(fmt) + "%s";

        std::string s = "[ ";
        for(; start != end; ++start)
            s += utils::strprintf(f.c_str(), *start, (start+1 == end) ? "" : ", ");
        s += " ]";

        return s;
    }

}

#endif

