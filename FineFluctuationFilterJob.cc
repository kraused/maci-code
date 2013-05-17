
// vi: tabstop=4:expandtab

#include "FineFluctuationFilterJob.hh"
#include "DesignByContract.hh"


FineFluctuationFilterJob::FineFluctuationFilterJob(linalg::SparseMatrix<double>* matrix)
: matrix(matrix)
{
    i_N = matrix->NumCols();
    i_type = MPI_DOUBLE;
    o_N = matrix->NumRows();
    o_type = MPI_DOUBLE;
}

void FineFluctuationFilterJob::exec(void* i_buf, void* o_buf)
{
    CHECK_PRECOND_ALWAYS(i_buf);
    CHECK_PRECOND_ALWAYS(o_buf);
    CHECK_PRECOND_ALWAYS(matrix);

    matrix->SpMV(linalg::SPMV_OP_NORMAL, 
                 1.0, 
                 (double* )i_buf,
                 0.0,
                 (double* )o_buf);
}

