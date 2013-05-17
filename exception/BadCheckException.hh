
// vi: tabstop=4:expandtab

#ifndef MACI_EXCEPTION_BAD_CHECK_EXCEPTION_HH_INCLUDED
#define MACI_EXCEPTION_BAD_CHECK_EXCEPTION_HH_INCLUDED 1

#include "exception/ExceptionBase.hh"

#include <string>

namespace exception {

    /// Indicates a failed check of a check in a function
    class BadCheckException : public ExceptionBase
    {

    public:
        /// Default constructor
        BadCheckException() throw();

        /// Create a new instance
        /// @param msg      the message
        /// @param file     the file where the exception is thrown
        /// @param line     the line where the exception is thrown
        /// @param file     the file where the exception is thrown
        /// @param funct    the line from which the exception is thrown
        BadCheckException(const std::string& msg,
                  const char *file, int line, const char *func) throw();

    };

}

#endif
