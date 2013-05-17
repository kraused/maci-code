
// vi: tabstop=4:expandtab

#include "exception/BadCallException.hh"

using namespace exception;

BadCallException::BadCallException() throw()
: ExceptionBase("[BadCallException] ")
{

}

BadCallException::BadCallException(const std::string& msg,
        const char *file, int line, const char *func) throw()
: ExceptionBase("[BadCallException] " + msg, file, line, func)
{
}

