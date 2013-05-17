
// vi: tabstop=4:expandtab

#ifndef MACI_EXCEPTION_BASE_HH_INCLUDED
#define MACI_EXCEPTION_BASE_HH_INCLUDED 1

#include <exception>
#include <string>

namespace exception {

    /// Base class for all exceptions
    class ExceptionBase : public std::exception
    {

    public:
        /// Default constructor
        ExceptionBase() throw();

        /// Create a new instance
        /// @param msg  the message
        ExceptionBase(const std::string& msg) throw(); 

        /// Create a new instance
        /// @param msg      the message
        /// @param file     the file where the exception is thrown
        /// @param line     the line where the exception is thrown
        /// @param file     the file where the exception is thrown
        /// @param funct    the line from which the exception is thrown
        ExceptionBase(const std::string& msg,
                  const char *file, int line, const char *func) throw();

        /// Destroy the instance
        ~ExceptionBase() throw();

        /// @overrides
        const char *what() const throw()
        {
            return message.c_str();
        }

    private:
        /// The error message
        std::string message;
    };

}


#endif

