
// vi: tabstop=4:expandtab

#ifndef MACI_TRUNCATED_LUMPED_MESH_MASS_MATRIX_HH_INCLUDED
#define MACI_TRUNCATED_LUMPED_MESH_MASS_MATRIX_HH_INCLUDED 1

#include "MeshMassMatrix.hh"


/// TruncatedLumpedMeshMassMatrix: Truncated lumped mesh mass
///                                matrix
class TruncatedLumpedMeshMassMatrix : public MeshMassMatrix
{

public:
    /// The default density is constant one
    TruncatedLumpedMeshMassMatrix(HandshakeGeo* handshake);
    TruncatedLumpedMeshMassMatrix(HandshakeGeo* handshake, Function<double, 3>* rho);

};

#endif

