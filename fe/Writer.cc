
// vi: tabstop=4:expandtab

#include "Writer.hh"

/* The MACI config file. Needed to
   know whether we can use PVO */
#include <config.hh>

#ifdef HAVE_PVO
#include "fe/PvoWriter.hh"
#endif

using namespace fe::interface;

Writer* Writer::New(Solver* solver)
{
#ifdef HAVE_PVO
    return new PvoWriter(solver);
#endif

    return NULL;
}

