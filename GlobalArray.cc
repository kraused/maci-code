
// vi: tabstop=4:expandtab

#include <ga.h>
#include <mpi.h>

#include "GlobalArray.hh"
#include "mpi/Communicator.hh"

#ifdef HAVE_GA
namespace
{

    struct GlobalArrayInitializer
    {

        GlobalArrayInitializer()
        {
            int mpi_is_init;

            MPI_Initialized(&mpi_is_init);
            CHECK_ALWAYS(1 == mpi_is_init, "MPI must be initialized before the MACI module is loaded");

            GA_Initialize();
        }

        ~GlobalArrayInitializer()
        {
            GA_Terminate();
        }

    } t;

}
#endif

