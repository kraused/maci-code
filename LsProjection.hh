
// vi: tabstop=4:expandtab

#ifndef MACI_LS_PROJECTION_HH_INCLUDED
#define MACI_LS_PROJECTION_HH_INCLUDED 1

#include "HandshakeGeo.hh"
#include "ProjectionBase.hh"

#include "fe/FiniteElements.hh"

/// Forwarding
class Maci;


/// The least squares projection matrix
class LsProjection : public ProjectionBase
{

public:
    /// Create a new instance
    /// @param[in]  app the Maci application instance
    LsProjection(Maci* app);

};

#endif

