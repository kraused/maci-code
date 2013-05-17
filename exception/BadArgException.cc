
// vi: tabstop=4:expandtab

#include "exception/BadArgException.hh"

using namespace exception;

BadArgException::BadArgException() throw()
: ExceptionBase("[BadArgException] ")
{

}

BadArgException::BadArgException(const std::string& msg,
        const char *file, int line, const char *func) throw()
: ExceptionBase("[BadArgException] " + msg, file, line, func)
{
}

