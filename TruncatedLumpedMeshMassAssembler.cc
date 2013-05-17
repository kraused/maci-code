
// vi: tabstop=4:expandtab

#include "TruncatedLumpedMeshMassAssembler.hh"
#include "DesignByContract.hh"
#include "HandshakeGeo.hh"


TruncatedLumpedMeshMassAssembler::TruncatedLumpedMeshMassAssembler(HandshakeGeo* handshake, Function<double, 3>* rho)
: handshake(handshake), rho(rho)
{
}

void TruncatedLumpedMeshMassAssembler::assemble()
{
    CHECK_PRECOND_ALWAYS(rho);

    if(PROC_FE == gcpe()->mytype)
        AssembleMatrix(handshake->elements(), *rho);

    gcpe()->world.barrier();
}

