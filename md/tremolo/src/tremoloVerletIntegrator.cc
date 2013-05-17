
// vi: tabstop=4:expandtab

#include "tremoloVerletIntegrator.hh"
#include "CallbackExtern.hh"


tremolo::VerletIntegrator::VerletIntegrator(Instance *self, double tau, ::TimeIntegrator* base)
: Base(self, tau, base)
{
}

void tremolo::VerletIntegrator::Run(int nsteps)
{
    TimeIntegrator_plugins_pre_integrationCB(base);

    TimeIntegrator_plugins_pre_forceCB(base);
    tremolo::instance_calc_force(app);
    TimeIntegrator_ApplyDamperCB(base);
    TimeIntegrator_plugins_post_forceCB(base);

    for(int i = 0; i < nsteps; ++i) 
    {
        TimeIntegrator_plugins_pre_stepCB(base);

        tremolo::instance_next_pre  (app);
        verlet_kick_drift();
        tremolo::instance_fix       (app);

        TimeIntegrator_plugins_pre_forceCB(base);
        tremolo::instance_calc_force(app);
        TimeIntegrator_ApplyDamperCB(base);
        TimeIntegrator_plugins_post_forceCB(base);

        verlet_kick();
        tremolo::instance_next_post (app);
        
        TimeIntegrator_plugins_post_stepCB(base);
    }

    TimeIntegrator_plugins_post_integrationCB(base);
}

