
// vi: tabstop=4:expandtab

#ifndef MACI_LS_UMFPACK_MULTIPLIER_MATRIX_HH
#define MACI_LS_UMFPACK_MULTIPLIER_MATRIX_HH

#include "UmfpackMultiplierMatrix.hh"
#include "HandshakeGeo.hh"
#include "Function.hh"

/// Forwarding
class Maci;


/// Least squares multiplier matrix which is inverted using the serial UMFPACK solver
class LsUmfpackMultiplierMatrix : public UmfpackMultiplierMatrix
{

public:
    /// Create a new instance
    /// @param[in]  app the Maci application instance
    LsUmfpackMultiplierMatrix(Maci* app);

};

#endif


