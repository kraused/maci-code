
// vi: tabstop=4:expandtab

#include "Writer.hh"

/* The MACI config file. Needed to
   know whether we can use PVO */
#include <config.hh>

#ifdef HAVE_PVO
#include "md/PvoWriter.hh"
#endif

using namespace md::interface;

Writer* Writer::New(System* system)
{
#ifdef HAVE_PVO
    return new PvoWriter(system);
#endif

    return NULL;
}

