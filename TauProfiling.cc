
// vi: tabstop=4:expandtab

#include <stdio.h>

#include "TauProfiling.hh"
#include "mpi/Communicator.hh"

#ifdef HAVE_TAU
namespace
{

    TAU_PROFILE_DECLARE_TIMER(app_timer)

    struct MaciTauInitializer
    {

        /// Constructor: Since x is a static member it is initialized
        /// when this shared library is opened and this constructor is
        /// automatically called
        MaciTauInitializer()
        {
            /// Initialize TAU. Since we do not have argc and argv from the
            /// main() function we fake it ...
            int argc = 1;
            const char* argv[] = { "maci", NULL };

            TAU_INIT(&argc, (char*** )&argv);

            /// TAU might not be able to know the id of the PE so that it will
            /// write all the data to profile.-1.0.0 which is bad.
            TAU_PROFILE_SET_NODE(mpi::Communicator(MPI_COMM_WORLD).myrank());

            /// Use a timer to measure the whole execution time from the initialization
            /// to the end
            TAU_PROFILE_CREATE_TIMER(app_timer, "application", "", TAU_DEFAULT);
            TAU_PROFILE_START(app_timer);
        }

        ~MaciTauInitializer()
        {
            TAU_PROFILE_STOP(app_timer);
            TAU_PROFILE_EXIT("");
        }

    } t;

}
#endif


