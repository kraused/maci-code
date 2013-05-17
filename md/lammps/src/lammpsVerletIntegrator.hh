
// vi: tabstop=4:expandtab

#ifndef MD_LAMMPS_SRC_LAMMPS_VERLET_INTEGRATOR_HH_INCLUDED
#define MD_LAMMPS_SRC_LAMMPS_VERLET_INTEGRATOR_HH_INCLUDED 1

#include "md/lammps/src/lammpsTimeIntegrator.hh"


namespace lammps 
{
    
    /// VerletIntegrator: verlet integrator
    class VerletIntegrator : public lammps::TimeIntegrator
    {
        typedef lammps::TimeIntegrator Base;

    public:
        VerletIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg);

        /// ? Returns the supported functionality
        int setmask();

        /// Called at the very beginning of each timestep. 
        /// Executes a verlet kick and drift and the 
        virtual void initial_integrate(int);

        /// Called at the end of each timestep.
        virtual void final_integrate();

        /// Called after pair and molecular forces are computed. Adds
        /// damping forces to the calculated forces
        virtual void post_force(int);

    private:
        /// Timestep size
        double tau;
    };

    /** Create a new VerletIntegrator
     */
    LAMMPS_NS::Fix* NewVerletIntegrator(LAMMPS_NS::LAMMPS *, int, char **);

}

#endif

