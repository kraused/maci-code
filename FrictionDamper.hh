
// vi: tabstop=4:expandtab

#ifndef MACI_FRICTION_DAMPER_HH_INCLUDED
#define MACI_FRICTION_DAMPER_HH_INCLUDED 1

#include "DampingMethod.hh"
#include "Function.hh"
#include "ParticleList.hh"

/// Forwarding
class Maci;


/// FrictionDamper: Implements the friction damping technique for
///            the particle system
class FrictionDamper : public DampingMethod
{

public:
    /// Create a new instance
    /// @param[in]  app     the Maci application instance
    /// @param[in]  d   The damping coefficients
    FrictionDamper(Maci* app, const Function<double,3>* D);

    /// Compute the dissipative force
    /// @param[out] F   the force term to be added to the particle
    ///         forces
    void ComputeDissipativeForce(std::valarray<double>* F);

};

#endif

