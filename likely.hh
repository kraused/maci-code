
// vi: tabstop=4:expandtab

#ifndef MACI_LIKELY_HH_INCLUDED
#define MACI_LIKELY_HH_INCLUDED 1

#undef   likely
#undef unlikely

#ifdef __GNUC__
/// Indicates to the compiler that the expression will typically be true
#define   likely(x) __builtin_expect((x),1)
/// Indicates to the compiler that the expression will typically be false
#define unlikely(x) __builtin_expect((x),0)
#else
#define   likely(x) x
#define unlikely(x) x
#endif

#endif


