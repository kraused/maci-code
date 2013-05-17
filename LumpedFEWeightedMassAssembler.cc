
// vi: tabstop=4:expandtab

#include "LumpedFEWeightedMassAssembler.hh"
#include "HandshakeGeo.hh"


LumpedFEWeightedMassAssembler::LumpedFEWeightedMassAssembler(HandshakeGeo* handshake)
: handshake(handshake)
{
}

void LumpedFEWeightedMassAssembler::assemble()
{
    AssembleMatrixUsingFEWeightedMass(handshake->nodes());
}

