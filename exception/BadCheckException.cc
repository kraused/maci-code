
// vi: tabstop=4:expandtab

#include "exception/BadCheckException.hh"

using namespace exception;

BadCheckException::BadCheckException() throw()
: ExceptionBase("[BadCheckException] ")
{

}

BadCheckException::BadCheckException(const std::string& msg,
                         const char *file, int line, const char *func) throw()
: ExceptionBase("[BadCheckException] " + msg, file, line, func)
{
}

