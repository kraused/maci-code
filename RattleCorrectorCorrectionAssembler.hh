
// vi: tabstop=4:expandtab

#ifndef MACI_RATTLE_CORRECTOR_CORRECTION_ASSEMBLER_HH_INCLUDED
#define MACI_RATTLE_CORRECTOR_CORRECTION_ASSEMBLER_HH_INCLUDED 1

#include "Assembler.hh"


/// RattleCorrectorCorrectionAssembler: Assembles the correction
/// matrix for the RATTLE 
class RattleCorrectorCorrectionAssembler : public Assembler
{

public:
    RattleCorrectorCorrectionAssembler(Assembler* meshMassAssembler, Assembler* constraintsMassAssembler);

    /// @overrides
    void assemble();

private:
    /// The assembler for the FE weighted mass matrix
    Assembler* meshMassAssembler;
    /// The assembler for the constraints mass matrix
    Assembler* constraintsMassAssembler;

};

#endif

