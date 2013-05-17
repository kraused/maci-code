
// vi: tabstop=4:expandtab

#include "exception/BadPrecondException.hh"

using namespace exception;

BadPrecondException::BadPrecondException() throw()
: ExceptionBase("[BadPrecondException] ")
{

}

BadPrecondException::BadPrecondException(const std::string& msg,
                         const char *file, int line, const char *func) throw()
: ExceptionBase("[BadPrecondException] " + msg, file, line, func)
{
}

