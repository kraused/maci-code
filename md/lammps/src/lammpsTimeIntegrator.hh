
// vi: tabstop=4:expandtab

#ifndef MD_LAMMPS_SRC_LAMMPS_TIME_INTEGRATOR_HH_INCLUDED
#define MD_LAMMPS_SRC_LAMMPS_TIME_INTEGRATOR_HH_INCLUDED 1

#include "fix.h"

/// Forwarding
class TimeIntegrator;


namespace lammps 
{
    
    /// TimeIntegrator: Base class for all time integrators
    class TimeIntegrator : public LAMMPS_NS::Fix
    {

    public:
        TimeIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg);

        /// ? Returns the supported functionality
        virtual int setmask();

        /// Initialization function
        virtual void init();

        /// Reset timestep
        virtual void reset_dt();

        /// Verlet kick
        void verlet_kick();

        /// Verlet drift
        void verlet_drift();

        /// Verlet kick and drift (merged loop version)
        void verlet_kick_drift();

        /// Set the base variable
        inline void setBase(::TimeIntegrator *b)
        {
            base = b;
        }

        virtual void pre_integration();
        virtual void post_integration();

        virtual void end_of_step();
        virtual void pre_force(int);

    private:
        /// Timestep size
        double dt;
        /// Timestep size for kick in a verlet step
        double dtf;

    protected:
        /// The time integrator as defined in the main code is primarily
        /// responsible for maintaining plugins and the execution of
        /// them at the end of each timestep
        ::TimeIntegrator *base;
    };

}

#endif

