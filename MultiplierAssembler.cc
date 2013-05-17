
// vi: tabstop=4:expandtab

#include "MultiplierAssembler.hh"
#include "Int64.hh"
#include "ProcElement.hh"
#include "ParallelMatMul.hh"
#include "linalg/CoordMatrix.hh"
#include "mpi/Cartcommunicator.hh"

MultiplierAssembler::MultiplierAssembler(Assembler* scaleTransferAssembler,
                                         Assembler* atomMassAssembler,
                                         Assembler* constraintsMassAssembler,
                                         Assembler* meshMassAssembler)
: scaleTransferAssembler(scaleTransferAssembler),
  atomMassAssembler(atomMassAssembler),
  constraintsMassAssembler(constraintsMassAssembler),
  meshMassAssembler(meshMassAssembler)
{
}

void MultiplierAssembler::assemble()
{
    linalg::CoordMatrix<int64,double> inverseMeshMasses, iMeshMassesxMprime, inverseAtomMasses, iAtomMassesxRprime;

    /// Step 1: Compute inv(M)*(M_constraints)'
    inverseMeshMasses = meshMassAssembler->matrix;
    inverseMeshMasses.InvertVals();

    constraintsMassAssembler->matrix.Transpose();
    ParallelMatMul(1.0, inverseMeshMasses, constraintsMassAssembler->matrix, 0.0, &iMeshMassesxMprime);
    constraintsMassAssembler->matrix.Transpose();
   
    /// Step 2: Compute (M_constraints)*inv(M)*(M_constraints)' and store it in
    ///         matrix 
    ParallelMatMul(1.0, constraintsMassAssembler->matrix, iMeshMassesxMprime, 0.0, &matrix);

    /// Step 3: Compute inv(M_atom)*R'
    inverseAtomMasses = atomMassAssembler->matrix;
    inverseAtomMasses.InvertVals();

    scaleTransferAssembler->matrix.Transpose();
    ParallelMatMul(1.0, inverseAtomMasses, scaleTransferAssembler->matrix, 0.0, &iAtomMassesxRprime);
    scaleTransferAssembler->matrix.Transpose();

    /// Step 4: Build R*inv(M_atom)*R' and add it to the matrix
    ParallelMatMul(1.0, scaleTransferAssembler->matrix, iAtomMassesxRprime, 1.0, &matrix);
    
    if(matrix.size() > 0)
    {
        int64 q;

        q = matrix.getMinRowIdx();
        CHECK_ALWAYS(gcpe()->world.myrank() == Int64::hi(q) && 0 == Int64::lo(q), 0);
        q = matrix.getMinColIdx();
        CHECK_ALWAYS(q <= Int64::concat(gcpe()->world.size(), 0), 0);
    }
}

