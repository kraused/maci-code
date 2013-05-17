
// vi: tabstop=4:expandtab

#ifndef MACI_NOOP_DAMPER_HH_INCLUDED
#define MACI_NOOP_DAMPER_HH_INCLUDED 1

#include "DampingMethod.hh"
#include "Function.hh"
#include "ParticleList.hh"

/// Forwarding
class Maci;


/// NoopDamper: A damper which does nothing
class NoopDamper : public DampingMethod
{

public:
    /// Initialize nothing
    /// @param[in]  app     the Maci application instance
    NoopDamper(Maci* app);
    
    /// Do nothing ...
    void ComputeDissipativeForce(std::valarray<double>* F);

};

#endif

