
// vi: tabstop=4:expandtab

#include "RattleCorrectorLsLumpedCorrectionMatrix.hh"
#include "LsAssembler.hh"
#include "LsLumpedMeshMassAssembler.hh"
#include "LumpedFEWeightedMassAssembler.hh"
#include "RattleCorrectorCorrectionAssembler.hh"


RattleCorrectorLsLumpedCorrectionMatrix::RattleCorrectorLsLumpedCorrectionMatrix(HandshakeGeo* handshake)
{
    AddSecondaryAssemblerIfNotExisting("LsTransfer",
                                       new LsAssembler(*handshake));
    AddSecondaryAssemblerIfNotExisting("LsLumpedMeshMassMatrix",
                                       new LsLumpedMeshMassAssembler(handshake,
                                                                     GetSecondaryAssembler("LsTransfer")));
    AddSecondaryAssemblerIfNotExisting("LumpedFEWeightedMassMatrix",
                                        new LumpedFEWeightedMassAssembler(handshake));
    
    AddPrimaryAssembler("RattleCorrectorLsLumpedCorrectionMatrix",
                        new RattleCorrectorCorrectionAssembler(GetSecondaryAssembler("LumpedFEWeightedMassMatrix"),
                                                               GetSecondaryAssembler("LsLumpedMeshMassMatrix")));
    CreateMatrix();
}

