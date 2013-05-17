
// vi: tabstop=4:expandtab

#include "ugVerletIntegrator.hh"
#include "CallbackExtern.hh"


UG_IF::VerletIntegrator::VerletIntegrator(SolverImplementation *solver, double tau, ::TimeIntegrator* base)
: Base(solver, tau, base)
{
}

void UG_IF::VerletIntegrator::Run(int nsteps)
{
    TimeIntegrator_plugins_pre_integrationCB(base);

    TimeIntegrator_plugins_pre_forceCB(base);
    solver->CalculateForces(0.0);
    TimeIntegrator_plugins_post_forceCB(base);

    for(int i = 0; i < nsteps; ++i) 
    {
        TimeIntegrator_plugins_pre_stepCB(base);

        verlet_kick_drift();
        solver->EnforceDirichletBC(i*dt);

        TimeIntegrator_plugins_pre_forceCB(base);
        solver->CalculateForces   (i*dt);
        TimeIntegrator_plugins_post_forceCB(base);

        verlet_kick();

        TimeIntegrator_plugins_post_stepCB(base);
    }

    TimeIntegrator_plugins_post_integrationCB(base);
}

