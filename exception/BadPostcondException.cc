
// vi: tabstop=4:expandtab

#include "exception/BadPostcondException.hh"

using namespace exception;

BadPostcondException::BadPostcondException() throw()
: ExceptionBase("[BadPostcondException] ")
{

}

BadPostcondException::BadPostcondException(const std::string& msg,
                               const char *file, int line, const char *func) throw()
: ExceptionBase("[BadPostcondException] " + msg, file, line, func)
{
}

