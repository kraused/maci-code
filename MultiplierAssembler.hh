
// vi: tabstop=4:expandtab

#ifndef MACI_MULTIPLIER_ASSEMBLER_HH_INCLUDED
#define MACI_MULTIPLIER_ASSEMBLER_HH_INCLUDED

#include "Assembler.hh"


/// MultiplierAssembler: Assembler for the multiplier matrix
class MultiplierAssembler : public Assembler
{

public:
    MultiplierAssembler(Assembler* scaleTransferAssembler,
                        Assembler* atomMassAssembler,
                        Assembler* constraintsMassAssembler,
                        Assembler* meshMassAssembler);

    /// @overrides
    void assemble();

private:
    /// The assembler for the scale transfer
    Assembler* scaleTransferAssembler;
    /// The assembler for the atomistic mass matrix. Theses
    /// masses should be weighted because otherwise the method
    /// doesn't work very well.
    Assembler* atomMassAssembler;
    /// The assembler for the mass matrix as used
    /// in the computation of the residual
    Assembler* constraintsMassAssembler;
    /// The assembler for the FE mass matrix. Again, the
    /// masses should be weighted, otherwise the coupling
    /// will not work very well
    Assembler* meshMassAssembler;
};

#endif

