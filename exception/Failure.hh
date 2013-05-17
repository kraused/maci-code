
// vi: tabstop=4:expandtab

#ifndef MACI_EXCEPTION_FAILURE_HH_INCLUDED
#define MACI_EXCEPTION_FAILURE_HH_INCLUDED 1

#include "exception/ExceptionBase.hh"

#include <string>

namespace exception {

    /// Indicates a program failure of general type
    class Failure : public ExceptionBase
    {

    public:
        /// Default constructor
        Failure() throw();

        /// Create a new instance
        /// @param msg      the message
        /// @param file     the file where the exception is thrown
        /// @param line     the line where the exception is thrown
        /// @param file     the file where the exception is thrown
        /// @param funct    the line from which the exception is thrown
        Failure(const std::string& msg,
            const char *file, int line, const char *func) throw();

    };

}

#endif

