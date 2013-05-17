
// vi: tabstop=4:expandtab

#include "RattleCorrectorTruncatedLumpedCorrectionMatrix.hh"
#include "TruncatedLumpedMeshMassAssembler.hh"
#include "LumpedFEWeightedMassAssembler.hh"
#include "RattleCorrectorCorrectionAssembler.hh"
#include "HandshakeGeo.hh"
#include "ConstantFunction.hh"


RattleCorrectorTruncatedLumpedCorrectionMatrix::RattleCorrectorTruncatedLumpedCorrectionMatrix(HandshakeGeo* handshake)
{
    ConstantFunction<double, 3> ones;

    AddSecondaryAssemblerIfNotExisting("TruncatedLumpedMeshMassMatrix",
                                       new TruncatedLumpedMeshMassAssembler(handshake, &ones));
    AddSecondaryAssemblerIfNotExisting("LumpedFEWeightedMassMatrix",
                                        new LumpedFEWeightedMassAssembler(handshake));

    AddPrimaryAssembler("RattleCorrectorTruncatedLumpedCorrectionMatrix",
                        new RattleCorrectorCorrectionAssembler(GetSecondaryAssembler("LumpedFEWeightedMassMatrix"),
                                                               GetSecondaryAssembler("TruncatedLumpedMeshMassMatrix")));
    CreateMatrix();
}

