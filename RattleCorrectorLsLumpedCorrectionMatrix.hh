
// vi: tabstop=4:expandtab

#ifndef MACI_RATTLE_CORRECTOR_LS_LUMPED_CORRECTION_MATRIX_HH_INCLUDED
#define MACI_RATTLE_CORRECTOR_LS_LUMPED_CORRECTION_MATRIX_HH_INCLUDED 1

#include "MeshMassMatrix.hh"

/// Forwarding
class HandshakeGeo;


/// RattleCorrectorLsLumpedCorrectionMatrix: Correction matrix for the 
/// RattleCorrector when the constraints mass matrix is a LsLumpedMeshMassMatrix
/// Even though this is not really a Finite Element mass matrix we derive
/// it from MeshMassMatrix
class RattleCorrectorLsLumpedCorrectionMatrix : public MeshMassMatrix
{

public:
    RattleCorrectorLsLumpedCorrectionMatrix(HandshakeGeo* handshake);

};

#endif

