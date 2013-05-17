
// vi: tabstop=4:expandtab

#include "lammpsTimeIntegrator.hh"
#include "lammpsInstance.hh"

#include "CallbackExtern.hh"

#include "update.h"
#include "atom.h"
#include "force.h"


lammps::TimeIntegrator::TimeIntegrator(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg)
: LAMMPS_NS::Fix(lmp, narg, arg), base(0)
{
    nevery         = 1;
    time_integrate = 1;
}

int lammps::TimeIntegrator::setmask()
{
    return END_OF_STEP | PRE_FORCE;
}

void lammps::TimeIntegrator::init()
{
    dt  = update->dt;
    dtf = 0.5*update->dt*force->ftm2v;
}

void lammps::TimeIntegrator::reset_dt()
{
    dt  = update->dt;
    dtf = 0.5*update->dt*force->ftm2v;
}

/// Kick a single particle
/// @param[in]  scal    scaling factor consisting of timestep and mass and 
///         weighting
inline void particle_kick (double *__restrict/*RESTRICT*/ v, 
               double *__restrict/*RESTRICT*/ f,
               double scal)
{
    v[0] += scal*f[0];
    v[1] += scal*f[1];
    v[2] += scal*f[2];
}

/// Let a single particle drift
/// @param[in]  scal    scaling factor consisting of timestep and mass and 
///         weighting
inline void particle_drift(double *__restrict/*RESTRICT*/ x, 
               double *__restrict/*RESTRICT*/ v,
               double scal)
{
    x[0] += scal*v[0];
    x[1] += scal*v[1];
    x[2] += scal*v[2];
}

/// Loop over all particles with matching mask
#define LOOP_PARTICLES(i, n, mask, groupbit)    \
    for(int i = 0; i < n; ++i)      \
        if(mask[i] & groupbit)

/// Short cut notation for getting the weighting of an
/// atom
#define WGT(i)  ( ((Piggyback* )atom->pb)[(i)].weight )

void lammps::TimeIntegrator::verlet_kick()
{
    double **v    = atom->v;
    double **f    = atom->f;
    double *mass  = atom->mass;
    double *rmass = atom->rmass;
    int    *type  = atom->type;
    int    nlocal = (igroup == atom->firstgroup) ? atom->nfirst : atom->nlocal;

        /// Not very nice but since the lmp variable comes from Lammps there is
    /// no other way than this or modifying the Lammps code.
    bool   wgt_enabled = static_cast<lammps::Instance*>(lmp)->Wgt_enabled();


    if(rmass) {
        if(wgt_enabled) {
            LOOP_PARTICLES(i, nlocal, atom->mask, groupbit)
                particle_kick(v[i], f[i], dtf/(WGT(i)*rmass[i]));
        } else {
            LOOP_PARTICLES(i, nlocal, atom->mask, groupbit)
                particle_kick(v[i], f[i], dtf/rmass[i]);
        }
    } else {
        if(wgt_enabled) {
            LOOP_PARTICLES(i, nlocal, atom->mask, groupbit)
                particle_kick(v[i], f[i], dtf/(WGT(i)*mass[type[i]]));
        } else {
            LOOP_PARTICLES(i, nlocal, atom->mask, groupbit)
                particle_kick(v[i], f[i], dtf/mass[type[i]]);
        }
    }
}

void lammps::TimeIntegrator::verlet_drift()
{
    double **x    = atom->x;
    double **v    = atom->v;
    int    nlocal = (igroup == atom->firstgroup) ? atom->nfirst : atom->nlocal;

    LOOP_PARTICLES(i, nlocal, atom->mask, groupbit) {
        particle_drift(x[i], v[i], dt);
    }
}

void lammps::TimeIntegrator::verlet_kick_drift()
{
    double **x    = atom->x;
    double **v    = atom->v;
    double **f    = atom->f;
    double *mass  = atom->mass;
    double *rmass = atom->rmass;
    int    *type  = atom->type;
    int    nlocal = (igroup == atom->firstgroup) ? atom->nfirst : atom->nlocal;

    /// Not very nice but since the lmp variable comes from Lammps there is
    /// no other way than this or modifying the Lammps code.
    bool   wgt_enabled = static_cast<lammps::Instance*>(lmp)->Wgt_enabled();


    if(rmass) {
        if(wgt_enabled) {
            LOOP_PARTICLES(i, nlocal, atom->mask, groupbit) {
                particle_kick (v[i], f[i], dtf/(WGT(i)*rmass[i]));
                particle_drift(x[i], v[i], dt);
            }
        } 
        else 
        {
            LOOP_PARTICLES(i, nlocal, atom->mask, groupbit) {
                particle_kick (v[i], f[i], dtf/rmass[i]);
                particle_drift(x[i], v[i], dt);
            }
        }
    } 
    else 
    {
        if(wgt_enabled) {
            LOOP_PARTICLES(i, nlocal, atom->mask, groupbit) {
                particle_kick (v[i], f[i], dtf/(WGT(i)*mass[type[i]]));
                particle_drift(x[i], v[i], dt);
            }
        } 
        else 
        {
            LOOP_PARTICLES(i, nlocal, atom->mask, groupbit) {
                particle_kick (v[i], f[i], dtf/mass[type[i]]);
                particle_drift(x[i], v[i], dt);
            }
        }
    }
}

void lammps::TimeIntegrator::pre_integration()
{
    TimeIntegrator_plugins_pre_integrationCB(base);
}

void lammps::TimeIntegrator::post_integration()
{
    TimeIntegrator_plugins_post_integrationCB(base);
}

void lammps::TimeIntegrator::pre_force(int vflag)
{
    TimeIntegrator_plugins_pre_forceCB(base);
}

void lammps::TimeIntegrator::end_of_step()
{
    TimeIntegrator_plugins_post_stepCB(base);
}

