
// vi: tabstop=4:expandtab

#ifndef MACI_LS_LUMPED_MESH_MASS_MATRIX_HH_INCLUDED
#define MACI_LS_LUMPED_MESH_MASS_MATRIX_HH_INCLUDED 1

#include "MeshMassMatrix.hh"

/// Forwarding
class HandshakeGeo;


/// LsLumpedMeshMassMatrix: Least-squared lumped mesh mass matrix
class LsLumpedMeshMassMatrix : public MeshMassMatrix
{

public:
    LsLumpedMeshMassMatrix(HandshakeGeo* handshake);

};

#endif

