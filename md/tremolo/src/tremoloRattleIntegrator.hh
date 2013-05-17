
// vi: tabstop=4:expandtab

#ifndef MD_TREMOLO_SRC_TREMOLO_RATTLE_INTEGRATOR_HH_INCLUDED
#define MD_TREMOLO_SRC_TREMOLO_RATTLE_INTEGRATOR_HH_INCLUDED 1

#include "md/tremolo/src/tremoloTimeIntegrator.hh"


namespace tremolo 
{
    
    /// RattleIntegrator: Rattle integrator for constrained integration
    class RattleIntegrator : public tremolo::TimeIntegrator
    {
        typedef tremolo::TimeIntegrator Base;

    public:
        /// Create a new instance
        /// @param[in]  self    the application
        /// @param[in]  tau timestep size
        /// @param[in]  base    Pointer to the base TimeIntegrator instance
        RattleIntegrator(Instance *self, double tau, ::TimeIntegrator* base);

        /// Run the integrator
        /// @param[in]  nsteps  number of steps
        void Run(int nsteps);
    };

}

#endif

