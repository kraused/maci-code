
// vi: tabstop=4:expandtab

#include "TruncatedLumpedMeshMassMatrix.hh"
#include "ConstantFunction.hh"
#include "TruncatedLumpedMeshMassAssembler.hh"


TruncatedLumpedMeshMassMatrix::TruncatedLumpedMeshMassMatrix(HandshakeGeo* handshake)
{
    ConstantFunction<double, 3> ones;

    AddPrimaryAssembler("TruncatedLumpedMeshMassMatrix", new TruncatedLumpedMeshMassAssembler(handshake, &ones));
    CreateMatrix();

    /// The matrix depends on the chosen density function
    /// rho and and hence we would need to give the assembler
    /// a unique name. However, taking into account that
    /// assembling the mass matrix is cheap we rather delete
    /// the assembler and recreate it the next time.
    DeletePrimaryAssembler();

    /// TODO Would be nice to do a WriteMatrix("TruncatedLumpedMeshMassMatrix")
    ///      at this point
}

TruncatedLumpedMeshMassMatrix::TruncatedLumpedMeshMassMatrix(HandshakeGeo* handshake, Function<double, 3>* rho)
{
    AddPrimaryAssembler("TruncatedLumpedMeshMassMatrix", new TruncatedLumpedMeshMassAssembler(handshake, rho));
    CreateMatrix();

    /// The matrix depends on the chosen density function
    /// rho and and hence we would need to give the assembler
    /// a unique name. However, taking into account that
    /// assembling the mass matrix is cheap we rather delete
    /// the assembler and recreate it the next time.
    DeletePrimaryAssembler();

    /// TODO Would be nice to do a WriteMatrix("TruncatedLumpedMeshMassMatrix")
    ///      at this point
}

