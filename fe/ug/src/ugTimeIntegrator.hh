
// vi: tabstop=4:expandtab

#ifndef MACI_FE_UG_SRC_UG_TIME_INTEGRATOR_HH_INCLUDED
#define MACI_FE_UG_SRC_UG_TIME_INTEGRATOR_HH_INCLUDED 1

#include "fe/ug/src/SolverImplementation.hh"

/// Forwarding
class TimeIntegrator;


namespace UG_IF
{
    
    /// TimeIntegrator: Base class for all time integrators
    class TimeIntegrator
    {

    public:
        /// Create a new integrator
        /// @param[in]  solver  The solver implemenation
        /// @param[in]  tau timestep size
        /// @param[in]  base    Pointer to the base TimeIntegrator instance
        TimeIntegrator(SolverImplementation *solver, double tau, ::TimeIntegrator* base);

        /// Verlet kick
        void verlet_kick();

        /// Verlet drift
        void verlet_drift();

        /// Verlet kick and drift (merged loop version)
        void verlet_kick_drift();

    protected:
        /// Pointer to the solver instance
        SolverImplementation* solver;
        /// Timestep size
        double    dt;
        /// Timestep size for kick in a verlet step
        double    dtf;
        /// The time integrator as defined in the main code is primarily
        /// responsible for maintaining plugins and the execution of
        /// them at the end of each timestep
        ::TimeIntegrator *base;
    };

}

#endif

