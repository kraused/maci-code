
// vi: tabstop=4:expandtab

#include "TauProfiling.hh"
#include "Int64.hh"
#include "DesignByContract.hh"
#ifndef NDEBUG
#include "ProcElement.hh"
#endif

#include "linalg/SparseMatrix.hh"
#include "linalg/CoordMatrix.hh"
#include "linalg/CSRHelper.hh"


namespace linalg 
{

    template<>
    SparseMatrix<float>::SparseMatrix(int M, int N)
    : nrows(M), ncols(N)
    {
    }

    template<>
    SparseMatrix<double>::SparseMatrix(int M, int N)
    : nrows(M), ncols(N)
    {
    }

    template<>
    SparseMatrix<double>::~SparseMatrix()
    {
        if(ap || ai || av) /* Either they all are allocated or none */
        {
            delete[] ap;
            delete[] ai;
            delete[] av;
        }
    }

    template<typename Tp>
    void SparseMatrix<Tp>::SpMV(SpMV_Op op, Tp alpha, Tp* x, Tp beta, Tp* y)
    {
        TAU_PROFILE("SparseMatrix::SpMV", "", TAU_DEFAULT);

        switch(op)
        {
        case SPMV_OP_NORMAL:
            CSR_SpMV      (nrows, ncols, ap, ai, av, alpha, x, beta, y);
            break;
        case SPMV_OP_TRANS:
            CSR_SpMV_Trans(nrows, ncols, ap, ai, av, alpha, x, beta, y);
            break;
        default:
            CHECK_ALWAYS(1 == 0, "Should not get here");
        }
    }

    template void SparseMatrix<float >::SpMV(SpMV_Op, float , float *, float , float *);
    template void SparseMatrix<double>::SpMV(SpMV_Op, double, double*, double, double*);

    template<>
    template<typename Ti, typename Tq>
    void SparseMatrix<float>::Fill_from_Coo_blk_id_3(const CoordMatrix<Ti, Tq>& M)
    {
        Coo_blk_id_3_to_CSR(M, nrows, ncols, &nnz, &ap, &ai, &av);
    }

    template<>
    template<typename Ti, typename Tq>
    void SparseMatrix<double>::Fill_from_Coo_blk_id_3(const CoordMatrix<Ti, Tq>& M)
    {
        Coo_blk_id_3_to_CSR(M, nrows, ncols, &nnz, &ap, &ai, &av);
    }

    template void SparseMatrix<float >::Fill_from_Coo_blk_id_3(const CoordMatrix<int64, double>& M);
    template void SparseMatrix<double>::Fill_from_Coo_blk_id_3(const CoordMatrix<int64, double>& M);

    template<typename Tp>
    void SparseMatrix<Tp>::Report_stats(const char* name)
    {
        CSR_Report(name, nrows, ncols, nnz, ap, ai, av);
    }

    template void SparseMatrix<float >::Report_stats(const char*);
    template void SparseMatrix<double>::Report_stats(const char*);

}

