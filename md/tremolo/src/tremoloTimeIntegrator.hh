
// vi: tabstop=4:expandtab

#ifndef MD_TREMOLO_SRC_TREMOLO_TIME_INTEGRATOR_HH_INCLUDED
#define MD_TREMOLO_SRC_TREMOLO_TIME_INTEGRATOR_HH_INCLUDED 1

#include "md/tremolo/src/tremoloInstance.hh"

/// Forwarding
class TimeIntegrator;


namespace tremolo {
    
    /// TimeIntegrator: Base class for all time integrators
    class TimeIntegrator
    {

    public:
        /// Create a new integrator
        /// @param[in]  self    the application
        /// @param[in]  tau timestep size
        /// @param[in]  base    Pointer to the base TimeIntegrator instance
        TimeIntegrator(Instance *self, double tau, ::TimeIntegrator* base);

        /// Verlet kick
        void verlet_kick();

        /// Verlet drift
        void verlet_drift();

        /// Verlet kick and drift (merged loop version)
        void verlet_kick_drift();

    protected:
        /// Tremolo application
        Instance *app;
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

