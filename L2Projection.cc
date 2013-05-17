
// vi: tabstop=4:expandtab

#include "L2Projection.hh"
#include "L2Assembler.hh"
#include "TruncatedLumpedMeshMassAssembler.hh"
#include "NodalValueScatterer.hh"
#include "ConstantFunction.hh"


L2Projection::L2Projection(Maci* app, const float h[])
: ProjectionBase(app)
{
    ConstantFunction<double, 3> ones;

    AddSecondaryAssemblerIfNotExisting(L2UniqueAssemblerName("L2Transfer", h), 
                                       new L2Assembler(*handshake, h));
    AddSecondaryAssemblerIfNotExisting("TruncatedLumpedMeshMassMatrix", 
                                       new TruncatedLumpedMeshMassAssembler(handshake, &ones));

    AddPrimaryAssembler(L2UniqueAssemblerName("L2Projection", h), 
                        new ProjectionAssembler(GetSecondaryAssembler(L2UniqueAssemblerName("L2Transfer", h)), 
                                                GetSecondaryAssembler("FETruncLumpedMasses")));
    CreateMatrix();
}

