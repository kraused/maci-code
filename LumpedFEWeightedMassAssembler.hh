
// vi: tabstop=4:expandtab

#ifndef MACI_LUMPED_FE_WEIGHTED_MASS_ASSEMBLER_HH_INCLUDED
#define MACI_LUMPED_FE_WEIGHTED_MASS_ASSEMBLER_HH_INCLUDED 1

#include "LumpedMeshMassAssembler.hh"

/// Forwarding
class HandshakeGeo;


/// LumpedFEWeightedMassAssembler: Assembler for the lumped weighted
/// finite element matrix
class LumpedFEWeightedMassAssembler : public LumpedMeshMassAssembler
{

public:
    LumpedFEWeightedMassAssembler(HandshakeGeo* handshake);

    /// @overrides
    void assemble();

private:
    /// Pointer to the handshake geometry
    HandshakeGeo* handshake;

};

#endif

