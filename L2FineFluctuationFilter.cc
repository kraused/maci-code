
// vi: tabstop=4:expandtab

#include "L2FineFluctuationFilter.hh"
#include "InterpolationAssembler.hh"
#include "L2Assembler.hh"
#include "TruncatedLumpedMeshMassAssembler.hh"
#include "ProjectionAssembler.hh"
#include "FineFluctuationFilterAssembler.hh"
#include "ConstantFunction.hh"


L2FineFluctutationFilter::L2FineFluctutationFilter(Maci* app, const float h[])
: FineFluctuationFilter(app)
{
    ConstantFunction<double, 3> ones;

    AddSecondaryAssemblerIfNotExisting("Interpolation", 
                                       new InterpolationAssembler(*handshake));
    AddSecondaryAssemblerIfNotExisting(L2UniqueAssemblerName("L2Transfer", h), 
                                       new L2Assembler(*handshake, h));
    AddSecondaryAssemblerIfNotExisting("TruncatedLumpedMeshMassMatrix",
                                       new TruncatedLumpedMeshMassAssembler(handshake, &ones));
    AddSecondaryAssemblerIfNotExisting(L2UniqueAssemblerName("L2Projection", h),
                                       new ProjectionAssembler(GetSecondaryAssembler(L2UniqueAssemblerName("L2Transfer", h)),
                                                               GetSecondaryAssembler("TruncatedLumpedMeshMassMatrix")));

    /// Triggers call to assemble()
    AddPrimaryAssembler(L2UniqueAssemblerName("L2FineFluctutationFilter", h), 
                        new FineFluctuationFilterAssembler(GetSecondaryAssembler("Interpolation"),
                                                           GetSecondaryAssembler(L2UniqueAssemblerName("L2Projection", h))));
    CreateMatrix();
}

