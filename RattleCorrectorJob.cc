
// vi: tabstop=4:expandtab

#include "RattleCorrectorJob.hh"
#include "DesignByContract.hh"


RattleCorrectorJob::RattleCorrectorJob(TransferOperator* R, 
                                       MeshMassMatrix* M, 
                                       NodalValueBuffer<double>* u, 
                                       NodalValueBuffer<double>* g, 
                                       MultiplierMatrix* L)
: R(R), M(M), u(u), g(g), L(L)
{
    i_N = R->NumCols();
    i_type = MPI_DOUBLE;
    o_N = R->NumCols();
    o_type = MPI_DOUBLE;
}

void RattleCorrectorJob::exec(void* i_buf, void* o_buf)
{
    CHECK_PRECOND_ALWAYS(i_buf);
    CHECK_PRECOND_ALWAYS(o_buf);
    CHECK_PRECOND_ALWAYS(R);
    CHECK_PRECOND_ALWAYS(M);
    CHECK_PRECOND_ALWAYS(u);
    CHECK_PRECOND_ALWAYS(g);
    CHECK_PRECOND_ALWAYS(L);
    CHECK_PRECOND_ALWAYS(nu);

    /// Compute g = M*U - R*u
    M->SpMV(linalg::SPMV_OP_NORMAL,
            +1.0,
            &(*u)[0],
             0.0,
            &(*g)[0]);
    R->SpMV(linalg::SPMV_OP_NORMAL,
            -1.0,
            (double* )i_buf,
            +1.0,
            &(*g)[0]);
    
    /// Compute the Lagrange multiplier 
    L->solve(&(*g)[0], &(*nu)[0], g->size());

    /// Compute R'*nu
    R->SpMV(linalg::SPMV_OP_TRANS,
            +1.0,
            &(*nu)[0],
             0.0,
            (double* )o_buf);
}

