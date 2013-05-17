
// vi: tabstop=4:expandtab

#include "exception/BadPlaceException.hh"

using namespace exception;

BadPlaceException::BadPlaceException() throw()
: ExceptionBase("[BadPlaceException] ")
{

}

BadPlaceException::BadPlaceException(const std::string& msg,
        const char *file, int line, const char *func) throw()
: ExceptionBase("[BadPlaceException] " + msg, file, line, func)
{
}

