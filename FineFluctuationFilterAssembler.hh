
// vi: tabstop=4:expandtab

#ifndef MACI_FINE_FLUCTUATION_FILTER_ASSEMBLER_HH_INCLUDED
#define MACI_FINE_FLUCTUATION_FILTER_ASSEMBLER_HH_INCLUDED 1

#include "Assembler.hh"


/// FineFluctuationFilterAssembler: Assembler for the fine
///                                 fluctuation filter matrix
class FineFluctuationFilterAssembler : public Assembler
{

public:
    FineFluctuationFilterAssembler(Assembler* interpolationAssembler,
                                   Assembler* projectionAssembler);
    
    /// @overrides
    void assemble();

private:
    /// The assembler for the interpolation
    /// matrix
    Assembler* interpolationAssembler;
    /// The assember for the projection 
    Assembler* projectionAssembler;
};

#endif

