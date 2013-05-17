
// vi: tabstop=4:expandtab

#ifndef MACI_EXCEPTION_BADARGEXCEPTION_HH_INCLUDED
#define MACI_EXCEPTION_BADARGEXCEPTION_HH_INCLUDED 1

#include "exception/ExceptionBase.hh"

#include <string>

namespace exception {

    /// Indicates wrong or unsupported input arguments to a function
    class BadArgException : public ExceptionBase
    {
    
    public:
        /// Default constructor
        BadArgException() throw();

        /// Create a new instance
        /// @param msg      the message
        /// @param file     the file where the exception is thrown
        /// @param line     the line where the exception is thrown
        /// @param file     the file where the exception is thrown
        /// @param funct    the line from which the exception is thrown
        BadArgException(const std::string& msg,
                    const char *file, int line, const char *func) throw();

    };
}

#endif
