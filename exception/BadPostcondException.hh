
// vi: tabstop=4:expandtab

#ifndef MACI_EXCEPTION_BAD_POSTCOND_EXCEPTION_HH_INCLUDED
#define MACI_EXCEPTION_POSTCOND_EXCEPTION_HH_INCLUDED 1

#include "exception/ExceptionBase.hh"

#include <string>

namespace exception {

    /// Indicates a failed check of a postcondition in a function
    class BadPostcondException : public ExceptionBase
    {

    public:
        /// Default constructor
        BadPostcondException() throw();

        /// Create a new instance
        /// @param msg      the message
        /// @param file     the file where the exception is thrown
        /// @param line     the line where the exception is thrown
        /// @param file     the file where the exception is thrown
        /// @param funct    the line from which the exception is thrown
        BadPostcondException(const std::string& msg,
                     const char *file, int line, const char *func) throw();

    };

}

#endif

