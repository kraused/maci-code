
// vi: tabstop=4:expandtab

#include "LumpedFEWeightedMassMatrix.hh"
#include "LumpedFEWeightedMassAssembler.hh"


LumpedFEWeightedMassMatrix::LumpedFEWeightedMassMatrix(HandshakeGeo* handshake)
{
    AddPrimaryAssembler("LumpedFEWeightedMassMatrix", new LumpedFEWeightedMassAssembler(handshake));
    CreateMatrix();

    /// We delete the assembler for the mass matrix
    /// because several mass matrices with different
    /// weights might be created. Also, assembling 
    /// mass matrices is rather cheap (especially because
    /// they are lumped) so the overhead is okay
    DeletePrimaryAssembler();

    /// TODO Get WriteMatrix() to work again for lumped mass matrices
    ///      so that we can call
    ///         WriteMatrix("LumpedFEWeightedMassMatrix");
}

