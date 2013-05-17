
// vi: tabstop=4:expandtab

#ifndef MACI_TAU_PROFILING_HH_INCLUDED
#define MACI_TAU_PROFILING_HH_INCLUDED 1

#include "config.hh"

#ifdef HAVE_TAU
#include <TAU.h>

/// The following macros are directly taken from TAU.h
///
/// TAU_PROFILE_TIMER(timer, function_name, type, group)
/// TAU_PROFILE_START(timer)
/// TAU_PROFILE_STOP(timer)

#else

#define TAU_PROFILE_TIMER(timer, function_name, type, group)
#define TAU_PROFILE_START(timer)
#define TAU_PROFILE_STOP(timer)
#define TAU_PROFILE(function_name, type, group)

#endif

#endif


