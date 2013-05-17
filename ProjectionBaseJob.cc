
// vi: tabstop=4:expandtab

#include "ProjectionBaseJob.hh"
#include "DesignByContract.hh"


ProjectionBaseJob::ProjectionBaseJob(TransferOperator* P, NodalValueBuffer<double>* u)
: P(P), u(u)
{
    CHECK_PRECOND_ALWAYS(P);

    i_N = P->NumCols();
    i_type = MPI_DOUBLE;
    o_N = 0;
    o_type = MPI_DOUBLE;
}

void ProjectionBaseJob::exec(void* i_buf, void* o_buf)
{
    CHECK_PRECOND_ALWAYS(i_buf);
    CHECK_PRECOND_ALWAYS(P);
    CHECK_PRECOND_ALWAYS(u);


    P->SpMV(linalg::SPMV_OP_NORMAL,
            -1.0,
            (double* )i_buf,
             0.0,
            &(*u)[0]);
}

