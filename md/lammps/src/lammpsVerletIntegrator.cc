
// vi: tabstop=4:expandtab

#include "lammpsVerletIntegrator.hh"

#include "CallbackExtern.hh"


LAMMPS_NS::Fix* lammps::NewVerletIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg)
{
    return new VerletIntegrator(lmp, narg, arg);
}

lammps::VerletIntegrator::VerletIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg)
: TimeIntegrator(lmp, narg, arg)
{
}

int lammps::VerletIntegrator::setmask()
{
    return Base::setmask()   | 
           INITIAL_INTEGRATE | 
           FINAL_INTEGRATE   |
           POST_FORCE;
}

void lammps::VerletIntegrator::initial_integrate(int vflag)
{
    TimeIntegrator_plugins_pre_stepCB(base);
    verlet_kick_drift();
}

void lammps::VerletIntegrator::final_integrate()
{
    verlet_kick();
}

void lammps::VerletIntegrator::post_force(int vflag)
{
    TimeIntegrator_ApplyDamperCB(base);
    TimeIntegrator_plugins_post_forceCB(base);
}

