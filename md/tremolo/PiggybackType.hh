
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_TREMOLO_PIGGYBACK_HH_INCLUDED
#define MD_INTERFACE_TREMOLO_PIGGYBACK_HH_INCLUDED 1

namespace md {
    namespace interface {

        /// Piggyback data structure
        struct PiggybackType
        {
            /// Maximal number of target FE processors for one particle
            static const int MAX_FEPROCS = 8;
            /// Maximal number of target BD processors for one particle
            static const int MAX_BDPROCS = 8;
            /// The maximum of MAX_FEPROCS and MAX_BDPROCS. If additional targets
            /// and displacements (because of an additional communication step) is
            /// added this number must be adopted
            static const int MAX_TARGETS = 8;

            //
            int source;
            int bdIndex;
            int feprocs[MAX_FEPROCS];
            int fedispl[MAX_FEPROCS];
            int origin;
            int locIndex;
            int bdprocs[MAX_BDPROCS];
            int bddispl[MAX_BDPROCS];
            
            /* No floats */
            
            double  weight;
            double  dampingD;
        };

    }
}

#endif

