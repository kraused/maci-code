
// vi: tabstop=4:expandtab

#include "md/tremolo/src/tremoloTimeIntegrator.hh"

#include "TauProfiling.hh"


tremolo::TimeIntegrator::TimeIntegrator(Instance *self, double tau, ::TimeIntegrator* argBase)
: app(self), base(argBase)
{
    dt  =     tau;
    dtf = 0.5*tau;
}

/// Kick a single particle
/// @param[in]  scal    scaling factor consisting of timestep and mass and 
///         weighting
inline void particle_kick (Particle *p, double scal)
{
    p->u[0] += scal*p->F[0];
    p->u[1] += scal*p->F[1];
    p->u[2] += scal*p->F[2];
}

/// Let a single particle drift
/// @param[in]  scal    scaling factor consisting of timestep and mass and 
///         weighting
inline void particle_drift(Particle *p, double scal)
{
    p->x[0] += scal*p->u[0];
    p->x[1] += scal*p->u[1];
    p->x[2] += scal*p->u[2];
}

void tremolo::TimeIntegrator::verlet_kick()
{
    Particle *p;

    TAU_PROFILE("TimeIntegrator::verlet_kick", "", TAU_DEFAULT);

    if(app->Wgt_enabled()) {
        particles_for_each(app, p) {
            particle_kick(p, dtf/(p->ParDat->mass*p->piggyback.weight));
        }
    } else {
        particles_for_each(app, p) {
            particle_kick(p, dtf/p->ParDat->mass);
        }
    }
}

void tremolo::TimeIntegrator::verlet_drift()
{
    Particle *p;

    TAU_PROFILE("TimeIntegrator::verlet_drift", "", TAU_DEFAULT);

    particles_for_each(app, p) {
        particle_drift(p, dt);
    }
}

void tremolo::TimeIntegrator::verlet_kick_drift()
{
    Particle *p;

    TAU_PROFILE("TimeIntegrator::verlet_kick_drift", "", TAU_DEFAULT);

    if(app->Wgt_enabled()) {
        particles_for_each(app, p) {
            particle_kick (p, dtf/(p->ParDat->mass*p->piggyback.weight));
            particle_drift(p, dt);
        }
    } else {
        particles_for_each(app, p) {
            particle_kick (p, dtf/p->ParDat->mass);
            particle_drift(p, dt);
        }
    }
}

