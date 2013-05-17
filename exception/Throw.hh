
// vi: tabstop=4:expandtab

#ifndef MACI_EXCEPTION_THROW_HH_INCLUDED
#define MACI_EXCEPTION_THROW_HH_INCLUDED

#include "utils/strprintf.hh"

#include "exception/ExceptionBase.hh"
#include "exception/Failure.hh"
#include "exception/BadArgException.hh"
#include "exception/BadCallException.hh"
#include "exception/BadDefaultException.hh"
#include "exception/BadPlaceException.hh"
#include "exception/BadPrecondException.hh"
#include "exception/BadPostcondException.hh"
#include "exception/BadCheckException.hh"
#include "exception/Failure.hh"

/// Throw an exception
/// @param Class    the class to throw (without namespace prefix)
/// @param msg      the message
#define THROW(Class, msg, args...)              \
    do {                            \
        throw exception::Class(             \
            utils::strprintf(msg, ##args),      \
            __FILE__, __LINE__, __FUNCTION__);  \
    } while(0)


#endif

