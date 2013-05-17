
// vi: tabstop=4:expandtab

#ifndef MD_LAMMPS_SRC_LAMMPS_NO_OP_INTEGRATOR_HH_INCLUDED
#define MD_LAMMPS_SRC_LAMMPS_NO_OP_INTEGRATOR_HH_INCLUDED 1

#include "fix.h"


namespace lammps 
{

    /// NoOpIntegrator: A Time integrator which does nothing.
    /// Intended to trigger the construction of internal lammps constructs
    /// necessary e.g., to compute the reference total energy.
    class NoOpIntegrator: public LAMMPS_NS::Fix
    {

    public:
        NoOpIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg);

        /// ? Returns the supported functionality
        virtual int setmask();

        /// Initialization function
        virtual void init();

        /// Reset timestep
        virtual void reset_dt();

        /// Apply plugins
        virtual void end_of_step();

        /// Called at the very beginning of each timestep. 
        /// Executes a verlet kick and drift and the 
        virtual void initial_integrate(int);

        /// Called at the end of each timestep.
        virtual void final_integrate();

    };

    /// Create a new NoOpIntegrator
    LAMMPS_NS::Fix* NewNoOpIntegrator(LAMMPS_NS::LAMMPS *, int, char **);

}

#endif


