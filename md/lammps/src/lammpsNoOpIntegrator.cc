
// vi: tabstop=4:expandtab

#include "lammpsNoOpIntegrator.hh"


LAMMPS_NS::Fix* lammps::NewNoOpIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg)
{
    return new NoOpIntegrator(lmp, narg, arg);
}

lammps::NoOpIntegrator::NoOpIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg)
: LAMMPS_NS::Fix(lmp, narg, arg)
{
    nevery         = 1;
    time_integrate = 1;
}

int lammps::NoOpIntegrator::setmask()
{
    return END_OF_STEP | INITIAL_INTEGRATE | FINAL_INTEGRATE;
}

void lammps::NoOpIntegrator::initial_integrate(int vflag)
{
}

void lammps::NoOpIntegrator::final_integrate()
{
}

void lammps::NoOpIntegrator::init()
{
}

void lammps::NoOpIntegrator::reset_dt()
{
}

void lammps::NoOpIntegrator::end_of_step()
{
}

