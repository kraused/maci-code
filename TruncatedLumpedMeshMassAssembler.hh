
// vi: tabstop=4:expandtab

#ifndef MACI_TRUNCATED_LUMPED_MESH_MASS_ASSEMBLER_HH_INCLUDED
#define MACI_TRUNCATED_LUMPED_MESH_MASS_ASSEMBLER_HH_INCLUDED 1

#include "LumpedMeshMassAssembler.hh"
#include "Function.hh"

/// Forwarding
class HandshakeGeo;


/// TruncatedLumpedMeshMassAssembler: Lumped mass matrix truncated
/// at the handshake boundary
class TruncatedLumpedMeshMassAssembler : public LumpedMeshMassAssembler
{

public:
    TruncatedLumpedMeshMassAssembler(HandshakeGeo* handshake, Function<double, 3>* rho);

    /// @overrides
    void assemble();

private:
    /// Pointer to the handshake geometry
    HandshakeGeo* handshake;
    /// Density function
    Function<double, 3>* rho;

};

#endif

