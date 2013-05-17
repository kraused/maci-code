
// vi: tabstop=4:expandtab

#ifndef MACI_FRICTION_FINE_FLUCTUATION_DAMPER_HH_INCLUDED
#define MACI_FRICTION_FINE_FLUCTUATION_DAMPER_HH_INCLUDED 1

#include "DampingMethod.hh"
#include "Function.hh"
#include "FineFluctuationFilter.hh"

/// Forwarding
class Maci;


/// FrictionFineFluctuationFilter: Implements frictional damping (proportional to velocities)
///                                with a fine fluctuation filter being inserted.
class FrictionFineFluctuationDamper : public DampingMethod
{

public:
    /// Create a new instance
    /// @param[in]  app     the Maci application instance
    /// @param[in]  Q   The fine flucutation filter
    /// @param[in]  d   The damping coefficients
    FrictionFineFluctuationDamper(Maci* Q, FineFluctuationFilter* argQ, const Function<double,3>* D);

    /// Compute the dissipative force
    /// @param[out] F       the force term to be added to the particle
    ///                     forces
    void ComputeDissipativeForce(std::valarray<double>* F);

private:
    /// Reference to the FineFluctuationFilter instance
    FineFluctuationFilter* Q;

};

#endif

