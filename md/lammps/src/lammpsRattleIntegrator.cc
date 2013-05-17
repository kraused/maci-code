
// vi: tabstop=4:expandtab

#include "lammpsRattleIntegrator.hh"

#include "CallbackExtern.hh"


LAMMPS_NS::Fix* lammps::NewRattleIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg)
{
    return new RattleIntegrator(lmp, narg, arg);
}

lammps::RattleIntegrator::RattleIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg)
: TimeIntegrator(lmp, narg, arg)
{
}

void lammps::RattleIntegrator::initial_integrate(int vflag)
{
    TimeIntegrator_plugins_pre_stepCB(base);
    verlet_kick_drift();
    RattleIntegrator_CorrectAfterKickDriftCB((::RattleIntegrator* )base);
}

void lammps::RattleIntegrator::final_integrate()
{
    verlet_kick();
    RattleIntegrator_CorrectAfterKickCB((::RattleIntegrator* )base);
}

void lammps::RattleIntegrator::post_force(int vflag)
{
    TimeIntegrator_ApplyDamperCB(base);
    TimeIntegrator_plugins_post_forceCB(base);
}

int lammps::RattleIntegrator::setmask()
{
    return Base::setmask()   | 
           INITIAL_INTEGRATE | 
           FINAL_INTEGRATE   |
           POST_FORCE;
}

