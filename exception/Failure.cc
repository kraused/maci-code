
// vi: tabstop=4:expandtab

#include "exception/Failure.hh"

using namespace exception;

Failure::Failure() throw()
: ExceptionBase("[Failure] ")
{

}

Failure::Failure(const std::string& msg,
        const char *file, int line, const char *func) throw()
: ExceptionBase("[Failure] " + msg, file, line, func)
{
}

