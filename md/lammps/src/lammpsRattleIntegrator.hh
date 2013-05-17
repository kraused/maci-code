
// vi: tabstop=4:expandtab

#ifndef MD_LAMMPS_SRC_LAMMPS_RATTLE_INTEGRATOR_HH_INCLUDED
#define MD_LAMMPS_SRC_LAMMPS_RATTLE_INTEGRATOR_HH_INCLUDED 1

#include "md/lammps/src/lammpsTimeIntegrator.hh"


namespace lammps 
{
    
    /// RattleIntegrator: Rattle integrator for constrained integration
    class RattleIntegrator : public lammps::TimeIntegrator
    {
        typedef lammps::TimeIntegrator Base;

    public:
        RattleIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg);

        /// ? Returns the supported functionality
        int setmask();

        /// Called at the very beginning of each timestep. 
        /// Executes a verlet kick and drift and the 
        virtual void initial_integrate(int);

        /// Called after pair and molecular forces are computed. Adds
        /// damping forces to the calculated forces
        virtual void post_force(int);

        /// Called at the end of each timestep.
        virtual void final_integrate();

    private:
        /// Timestep size
        double tau;
    };

    /** Create a new RattleIntegrator
     */
    LAMMPS_NS::Fix* NewRattleIntegrator(LAMMPS_NS::LAMMPS *, int, char **);

}

#endif


