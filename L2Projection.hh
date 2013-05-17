
// vi: tabstop=4:expandtab

#ifndef MACI_L2_PROJECTION_HH_INCLUDED
#define MACI_L2_PROJECTION_HH_INCLUDED 1

#include "HandshakeGeo.hh"
#include "ProjectionBase.hh"

#include "fe/FiniteElements.hh"

/// Forwarding
class Maci;


/// The L2 projection matrix
class L2Projection : public ProjectionBase
{

public:
    /// Create a new instance
    /// @param[in] app  the Maci application instance
    /// @param[in] h    patch sizes (same for all particles)
    L2Projection(Maci* app, const float h[]);

};

#endif

