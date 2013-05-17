
// vi: tabstop=4:expandtab

#ifndef MACI_PROJECTION_ASSEMBLER_HH_INCLUDED
#define MACI_PROJECTION_ASSEMBLER_HH_INCLUDED 1

#include "Assembler.hh"


/// ProjectionAssembler: An assembler which constructs a 
///                      projection from a transfer matrix
///                      and a lumped mass matrix
class ProjectionAssembler : public Assembler
{

public:
    ProjectionAssembler(Assembler* scaleTransferAssembler,
                        Assembler* meshMassesAssembler);

    /// @overrides
    void assemble();

private:
    /// The transfer assembler
    Assembler* scaleTransferAssembler;
    /// The assembler for the mass matrix
    Assembler* meshMassesAssembler;
};

#endif

