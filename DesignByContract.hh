
// vi: tabstop=4:expandtab

#ifndef MACI_DESIGN_BY_CONTRACT_HH_INCLUDED
#define MACI_DESIGN_BY_CONTRACT_HH_INCLUDED 1

///
/// @file DesignByContract.hh
///
/// A collection of methods and macros to do compile and run-time checks.
///
/// I want to keep everything as simple as possible. Therefore I have only two
/// compilation modes, namely DEBUG and NDEBUG. Every check falls into one of
/// the following two categories
/// - check always: These are the tests which need to be made in production runs
///        e.g. because they can simply occur without buggy code
///        behavior or might be caused by wrong user input (from the
///        script side)
/// - check in DEBUG: Just like the usual assert
///
/// Every macro should have two variants, a variant which is executed always and
/// one variant which is only executed in DEBUG mode
///

#include "exception/Throw.hh"

#if !defined(DEBUG) && !defined(NDEBUG)
#error " None of the following macros is defined: DEBUG, NDEBUG"
#endif

#ifdef NDEBUG
#undef  OPTIMIZE
#define OPTIMIZE 1
#endif

/// Check a condition and abort the program if something wents wrong
/// @param expr the expression which either evaluates to true or to false.
///         It is illegal to do any operation in expr which changes the
///         program state (i.e. increment a variable, assignments, ...). The
///         number of evaulations of expr is not specified at all!
/// @param msg  A message to be displayed on failure. This can be some hint to
///         the user what might go wrong; can be NULL
#define CHECK_ALWAYS(expr, msg)                                     \
    if(!(expr))                                                     \
    {                                                               \
            THROW(BadCheckException,__STRING(expr) " (%s)", msg);   \
    }

#ifdef DEBUG
#define CHECK_DEBUG(expr, msg)  CHECK_ALWAYS(expr, msg)
#endif
#ifdef OPTIMIZE
#define CHECK_DEBUG(expr, msg)  static_cast<void>(0);
#endif

/// Check a precondition for a function call. I assume that the check is more or
/// less understandable so that no explanation is necessary
/// @param expr the argument check
#define CHECK_PRECOND_ALWAYS(expr)                                  \
    if(!(expr)) { THROW(BadPrecondException,__STRING(expr)); }

#ifdef DEBUG
#define CHECK_PRECOND_DEBUG(expr)   CHECK_PRECOND_ALWAYS(expr)
#endif
#ifdef OPTIMIZE
#define CHECK_PRECOND_DEBUG(expr)   static_cast<void>(0)
#endif

/// Check a postcondition for a function call. I assume that the check is more or
/// less understandable so that no explanation is necessary
/// @param expr the argument check
#define CHECK_POSTCOND_ALWAYS(expr)                                 \
    if(!(expr)) { THROW(BadPostcondException,__STRING(expr)); }

#ifdef DEBUG
#define CHECK_POSTCOND_DEBUG(expr)  CHECK_POSTCOND_ALWAYS(expr)
#endif
#ifdef OPTIMIZE
#define CHECK_POSTCOND_DEBUG(expr)  static_cast<void>(0)
#endif

#endif


