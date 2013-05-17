
// vi: tabstop=4:expandtab

#include "RattleCorrectorCorrectionAssembler.hh"
#include "ParallelMatMul.hh"


RattleCorrectorCorrectionAssembler::RattleCorrectorCorrectionAssembler(Assembler* meshMassAssembler, Assembler* constraintsMassAssembler)
: meshMassAssembler(meshMassAssembler), constraintsMassAssembler(constraintsMassAssembler)
{
}

void RattleCorrectorCorrectionAssembler::assemble()
{
    /// Copy the matrix so that we can invert the matrix
    AssemblerOutputMatrix M = meshMassAssembler->matrix;
    M.InvertVals();

    /// Transpose the contraints mass matrix. This is usually
    /// not necessary because mass matrices are symmetric but
    /// it doesn't cost much and maybe someday people want to
    /// use a non-symmetric constraints mass matrix (who knows).
    constraintsMassAssembler->matrix.Transpose();
    ParallelMatMul(1.0, M, constraintsMassAssembler->matrix, 0.0, &matrix);
    constraintsMassAssembler->matrix.Transpose();
}

