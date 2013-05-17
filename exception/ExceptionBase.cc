
// vi: tabstop=4:expandtab

#include "exception/ExceptionBase.hh"

#include <sstream>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

using namespace exception;

ExceptionBase::ExceptionBase() throw()
: message("Unknown")
{
}

ExceptionBase::ExceptionBase(const std::string& msg) throw()
: message(msg)
{
}

ExceptionBase::ExceptionBase(const std::string& msg,
                 const char *file, int line, const char *func) throw()
: std::exception()
{
    std::stringstream s;
    s << "At " << file << ":" << line << ": " << msg;
    message = s.str();
}

ExceptionBase::~ExceptionBase() throw()
{

}

