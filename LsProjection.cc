
// vi: tabstop=4:expandtab

#include "LsProjection.hh"
#include "LsAssembler.hh"
#include "LsLumpedMeshMassAssembler.hh"
#include "NodalValueScatterer.hh"


LsProjection::LsProjection(Maci* app)
: ProjectionBase(app)
{
    AddSecondaryAssemblerIfNotExisting("LsTransfer", new LsAssembler(*handshake));
    AddSecondaryAssemblerIfNotExisting("LsLumpedMeshMassMatrix",
                                       new LsLumpedMeshMassAssembler(this->handshake,
                                                                     this->GetSecondaryAssembler("LsTransfer")));

    AddPrimaryAssembler("LsProjection", new ProjectionAssembler(GetSecondaryAssembler("LsTransfer"), GetSecondaryAssembler("FELsTrunccLumpedMasses")));
    CreateMatrix();
}

