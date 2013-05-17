
// vi: tabstop=4:expandtab

#include "ResidualNormComputerJob.hh"
#include "DesignByContract.hh"


ResidualNormComputerJob::ResidualNormComputerJob(TransferOperator* R, 
                                                 MeshMassMatrix* M, 
                                                 NodalValueBuffer<double>* u, 
                                                 NodalValueBuffer<double>* g) 
: R(R), M(M), u(u), g(g)
{
    CHECK_PRECOND_ALWAYS(R);
    CHECK_PRECOND_ALWAYS(M);

    i_N = R->NumCols();
    i_type = MPI_DOUBLE;
    o_N = 0;
    o_type = MPI_DOUBLE;
}

void ResidualNormComputerJob::exec(void* i_buf, void* o_buf)
{
    CHECK_PRECOND_ALWAYS(i_buf);
    CHECK_PRECOND_ALWAYS(R);
    CHECK_PRECOND_ALWAYS(M);
    CHECK_PRECOND_ALWAYS(u);
    CHECK_PRECOND_ALWAYS(g);


    /// Compute g = M*U - R*u
    M->SpMV(linalg::SPMV_OP_NORMAL,
            +1.0,
            &(*u)[0],
             0.0,
            &(*g)[0]);
    R->SpMV(linalg::SPMV_OP_NORMAL,
            -1.0,
            (double* )i_buf,
             1.0,
            &(*g)[0]);
}

