
// vi: tabstop=4:expandtab

#include "VerletIntegrator.hh"
#include "Maci.hh"


VerletIntegrator::VerletIntegrator(Maci* app, DampingMethod* damper)
: TimeIntegrator(app)
{
    this->damper = damper;
}

VerletIntegrator::~VerletIntegrator()
{
}

