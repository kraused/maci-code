
// vi: tabstop=4:expandtab

#ifndef MACI_RATTLE_CORRECTOR_TRUNCATED_LUMPED_CORRECTION_MATRIX_HH_INCLUDED
#define MACI_RATTLE_CORRECTOR_TRUNCATED_LUMPED_CORRECTION_MATRIX_HH_INCLUDED 1

#include "MeshMassMatrix.hh"

/// Forwarding
class HandshakeGeo;


/// RattleCorrectorTruncatedLumpedCorrectionMatrix: Correction matrix for the 
/// RattleCorrector when the constraints mass matrix is a TruncatedLumpedMeshMassMatrix
/// Even though this is not really a Finite Element mass matrix we derive
/// it from MeshMassMatrix
class RattleCorrectorTruncatedLumpedCorrectionMatrix : public MeshMassMatrix
{

public:
    RattleCorrectorTruncatedLumpedCorrectionMatrix(HandshakeGeo* handshake);

};

#endif

