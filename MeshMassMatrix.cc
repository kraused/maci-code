
// vi: tabstop=4:expandtab

#include "MeshMassMatrix.hh"
#include "Delete.hh"
#include "DesignByContract.hh"


MeshMassMatrix::MeshMassMatrix()
: A(0)
{
}

MeshMassMatrix::~MeshMassMatrix()
{
    DeletePointer(A);
}

int MeshMassMatrix::NumRows() const
{
    if(A)
        return A->NumRows();
    else
        return 0;
}

int MeshMassMatrix::NumCols() const
{
    if(A)
        return A->NumCols();
    else
        return 0;
}

void MeshMassMatrix::SpMV(linalg::SpMV_Op op, double alpha, double* x, double  beta, double* b)
{
    if(A)
        A->SpMV(op, alpha, x, beta, b);
    /// else: do nothing
}

void MeshMassMatrix::CreateMatrix()
{
    AssemblerOutputMatrix M = GetPrimaryAssembler()->matrix;

    if(0 == M.size())
        goto fn_exit;

    for(AssemblerOutputMatrix::Iterator a = M.begin(); a != M.end(); ++a)
    {
        /// TODO Currently we only support diagonal matrices where
        ///      this is always true. For general matrices we need
        ///      to do more work!
        CHECK_ALWAYS(Int64::hi(a->row) == gcpe()->world.myrank(), 0);
        CHECK_ALWAYS(Int64::hi(a->col) == gcpe()->world.myrank(), 0);

        a->row = Int64::lo(a->row);
        a->col = Int64::lo(a->col);
    }

    A = new linalg::SparseMatrix<double>(3*M.getRelativeNumOfRows(),
                                         3*M.getRelativeNumOfCols());
    A->Fill_from_Coo_blk_id_3(M);

fn_exit:
    ;
}

