
// vi: tabstop=4:expandtab

#ifndef MACI_LS_LUMPED_MESH_MASS_ASSEMBLER_HH_INCLUDED
#define MACI_LS_LUMPED_MESH_MASS_ASSEMBLER_HH_INCLUDED 1

#include "Assembler.hh"

/// Forwarding
class HandshakeGeo;


/// LsLumpedMeshMassAssembler: Assembler for least squares mass matrix
/// We compute the mass matrix (lumped) as the row sum of the transfer operator
/// for simplicity.
class LsLumpedMeshMassAssembler : public Assembler
{

public:
    LsLumpedMeshMassAssembler(HandshakeGeo* handshake, Assembler* lsTransferAssembler);

    /// @overrides
    void assemble();

private:
    /// Pointer to the handshake geometry
    HandshakeGeo* handshake;
    /// The assembler for the least squares transfer
    /// operator
    Assembler* lsTransferAssembler;

};

#endif

