
// vi: tabstop=4:expandtab

#ifndef MACI_L2_UMFPACK_MULTIPLIER_MATRIX_HH
#define MACI_L2_UMFPACK_MULTIPLIER_MATRIX_HH

#include "UmfpackMultiplierMatrix.hh"
#include "HandshakeGeo.hh"
#include "Function.hh"

/// Forwarding
class Maci;


/// L2 Multiplier matrix which is inverted using the serial UMFPACK solver
class L2UmfpackMultiplierMatrix : public UmfpackMultiplierMatrix
{

public:
    /// Create a new instance
    /// @param[in]  app the Maci application instance
    /// @param[in] h    patch sizes (same for all particles)
    L2UmfpackMultiplierMatrix(Maci* app, const float h[]);

};

#endif

