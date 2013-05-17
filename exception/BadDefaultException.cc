
// vi: tabstop=4:expandtab

#include "exception/BadDefaultException.hh"

using namespace exception;

BadDefaultException::BadDefaultException() throw()
: ExceptionBase("[BadDefaultException] ")
{

}

BadDefaultException::BadDefaultException(const std::string& msg,
                         const char *file, int line, const char *func) throw()
: ExceptionBase("[BadDefaultException] " + msg, file, line, func)
{
}

