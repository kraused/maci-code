
// vi: tabstop=4:expandtab

#ifndef MACI_EXCEPTION_BADPLACEEXCEPTION_HH_INCLUDED
#define MACI_EXCEPTION_BADPLACEEXCEPTION_HH_INCLUDED 1

#include "exception/ExceptionBase.hh"

#include <string>

namespace exception {

    /// This idea is stolen from X10
    /// I use this exception to indicate a function call on a wrong type of
    /// processing element
    class BadPlaceException : public ExceptionBase
    {

    public:
        /// Default constructor
        BadPlaceException() throw();

        /// Create a new instance
        /// @param msg      the message
        /// @param file     the file where the exception is thrown
        /// @param line     the line where the exception is thrown
        /// @param file     the file where the exception is thrown
        /// @param funct    the line from which the exception is thrown
        BadPlaceException(const std::string& msg,
                      const char *file, int line, const char *func) throw();
 
    };

}

#endif

