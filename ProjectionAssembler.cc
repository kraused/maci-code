
// vi: tabstop=4:expandtab

#include "ProjectionAssembler.hh"
#include "ProcElement.hh"
#include "ParallelMatMul.hh"


ProjectionAssembler::ProjectionAssembler(Assembler* scaleTransferAssembler,
                                         Assembler* meshMassesAssembler)
: scaleTransferAssembler(scaleTransferAssembler), meshMassesAssembler(meshMassesAssembler)
{
}

void ProjectionAssembler::assemble()
{
    AssemblerOutputMatrix M = meshMassesAssembler->matrix;
    M.InvertVals();

    ParallelMatMul(1.0, M, scaleTransferAssembler->matrix, 0.0, &matrix);
}


