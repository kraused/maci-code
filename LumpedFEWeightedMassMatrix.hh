
// vi: tabstop=4:expandtab

#ifndef MACI_LUMPED_FE_WEIGHTED_MASS_MATRIX_HH_INCLUDED
#define MACI_LUMPED_FE_WEIGHTED_MASS_MATRIX_HH_INCLUDED 1

#include "MeshMassMatrix.hh"

/// Forwarding
class HandshakeGeo;


/// LumpedFEWeightedMassMatrix: Lumped weighted finite element matrix
class LumpedFEWeightedMassMatrix : public MeshMassMatrix
{

public:
    LumpedFEWeightedMassMatrix(HandshakeGeo* handshake);

};

#endif

