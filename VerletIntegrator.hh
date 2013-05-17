
// vi: tabstop=4:expandtab

#ifndef MACI_VERLET_INTEGRATOR_HH_INCLUDED
#define MACI_VERLET_INTEGRATOR_HH_INCLUDED 1

#include "TimeIntegrator.hh"

/// Forwarding
class Maci;
class DampingMethod;


/// VerletIntegrator: Implements the Verlet integration method
/// Verlet can be used to solve the uncoupled system of MD or FE in isolation
/// Obviously, this is much easier to do without using Maci but it can be
/// useful for debugging.
class VerletIntegrator : public TimeIntegrator
{

public:
    /// Create a new intestance
    /// @param[in]  app the Maci application instance
    /// @param[in]  damper  damping method to be applied during
    ///         integration
    VerletIntegrator(Maci* app, DampingMethod* damper);

    /// Destructor
    ~VerletIntegrator();

};

#endif

