
// vi: tabstop=4:expandtab

#include "LsFineFluctuationFilter.hh"
#include "InterpolationAssembler.hh"
#include "LsAssembler.hh"
#include "LsLumpedMeshMassAssembler.hh"
#include "ProjectionAssembler.hh"
#include "FineFluctuationFilterAssembler.hh"


LsFineFluctutationFilter::LsFineFluctutationFilter(Maci* app)
: FineFluctuationFilter(app)
{
    AddSecondaryAssemblerIfNotExisting("Interpolation", 
                                       new InterpolationAssembler(*handshake));
    AddSecondaryAssemblerIfNotExisting("LsTransfer", 
                                       new LsAssembler(*handshake));
    AddSecondaryAssemblerIfNotExisting("LsLumpedMeshMassMatrix",
                                       new LsLumpedMeshMassAssembler(handshake,
                                                                     this->GetSecondaryAssembler("LsTransfer")));
    AddSecondaryAssemblerIfNotExisting("LsProjection", 
                                       new ProjectionAssembler(GetSecondaryAssembler("LsTransfer"),
                                                               GetSecondaryAssembler("LsLumpedMeshMassMatrix")));

    /// Triggers call to assemble()
    AddPrimaryAssembler("LsFineFluctutationFilter", 
                        new FineFluctuationFilterAssembler(GetSecondaryAssembler("Interpolation"),
                                                           GetSecondaryAssembler("LsProjection")));
    CreateMatrix();
}

