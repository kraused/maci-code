
// vi: tabstop=4:expandtab

#ifndef MACI_FE_UG_SRC_UG_VERLET_INTEGRATOR_HH_INCLUDED
#define MACI_FE_UG_SRC_UG_VERLET_INTEGRATOR_HH_INCLUDED 1

#include "fe/ug/src/ugTimeIntegrator.hh"


namespace UG_IF 
{
    
    /// VerletIntegrator: Verlet integrator
    class VerletIntegrator : public UG_IF::TimeIntegrator
    {
        typedef UG_IF::TimeIntegrator Base;

    public:
        /// Create a new instance
        /// @param[in]  solver  pointer to the solver implementation
        /// @param[in]  tau timestep size
        /// @param[in]  base    Pointer to the base TimeIntegrator instance
        VerletIntegrator(SolverImplementation *solver, double tau, ::TimeIntegrator* base);

        /// Run the integrator
        /// @param[in]  nsteps  number of steps
        void Run(int nsteps);

    };

}

#endif

