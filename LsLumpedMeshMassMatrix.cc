
// vi: tabstop=4:expandtab

#include "LsLumpedMeshMassMatrix.hh"
#include "LsAssembler.hh"
#include "LsLumpedMeshMassAssembler.hh"


LsLumpedMeshMassMatrix::LsLumpedMeshMassMatrix(HandshakeGeo* handshake)
{
    AddSecondaryAssemblerIfNotExisting("LsTransfer",
                                       new LsAssembler(*handshake));
    AddPrimaryAssembler("LsLumpedMeshMassMatrix",
                        new LsLumpedMeshMassAssembler(handshake, GetSecondaryAssembler("LsTransfer")));
    CreateMatrix();

    /// TODO Would be nice to do a WriteMatrix("LsLumpedMeshMassMatrix")
    ///      at this point
}

