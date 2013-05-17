
// vi: tabstop=4:expandtab

#include "TauProfiling.hh"
#include "Int64.hh"
#include "DesignByContract.hh"
#include "Logging.hh"

#include "linalg/SparseMatrix.hh"
#include "linalg/CoordMatrix.hh"


namespace linalg 
{

    template<>
    SparseMatrix<float>::SparseMatrix(int M, int N)
    : nrows(M), ncols(N), A(BLAS_suscr_begin(M,N))
    {
    }

    template<>
    SparseMatrix<double>::SparseMatrix(int M, int N)
    : nrows(M), ncols(N), A(BLAS_duscr_begin(M,N))
    {
    }

    template<typename Tp>
    SparseMatrix<Tp>::~SparseMatrix()
    {
        BLAS_usds(A);
    }

    template SparseMatrix<float >::~SparseMatrix();
    template SparseMatrix<double>::~SparseMatrix();

    template<typename Tp>
    static void scal(Tp* x, int N, float beta)
    {
        long i;
        
        if(0.0 == beta)
            std::fill(x, x+N, 0.0);
        else
        if(1.0 == beta)
            ;   /// Do nothing
        else
            for(i = 0; i < N; ++i)
                x[i] *= beta;
    }

    template<>
    void SparseMatrix<float>::SpMV(SpMV_Op op, float alpha, float* x, float beta, float* y)
    {
        TAU_PROFILE("SparseMatrix<float>::SpMV", "", TAU_DEFAULT);

        switch(op)
        {
        case SPMV_OP_NORMAL:
            scal(y, nrows, beta);
            BLAS_susmv(blas_no_trans, alpha, A, x, 1, y, 1);
            break;
        case SPMV_OP_TRANS:
            scal(y, ncols, beta);
            BLAS_susmv(blas_trans   , alpha, A, x, 1, y, 1);
            break;
        default:
            CHECK_ALWAYS(1 == 0, 0);
        }
    }

    template<>
    void SparseMatrix<double>::SpMV(SpMV_Op op, double alpha, double* x, double beta, double* y)
    {
        TAU_PROFILE("SparseMatrix<double>::SpMV", "", TAU_DEFAULT);

        switch(op)
        {
        case SPMV_OP_NORMAL:
            scal(y, nrows, beta);
            BLAS_dusmv(blas_no_trans, alpha, A, x, 1, y, 1);
            break;
        case SPMV_OP_TRANS:
            scal(y, ncols, beta);
            BLAS_dusmv(blas_trans   , alpha, A, x, 1, y, 1);
            break;
        default:
            CHECK_ALWAYS(1 == 0, 0);
        }
    }

    template<>
    template<typename Ti, typename Tq>
    void SparseMatrix<float>::Fill_from_Coo_blk_id_3(const CoordMatrix<Ti, Tq>& M)
    {
        for(size_t i = 0; i < M.size(); ++i)
        {
            CHECK_DEBUG((int )3*M[i].row+2 >= 0, 0);
            CHECK_DEBUG((int )3*M[i].col+2 >= 0, 0);

            BLAS_suscr_insert_entry(A, M[i].val, 3*M[i].row+0, 3*M[i].col+0);
            BLAS_suscr_insert_entry(A, M[i].val, 3*M[i].row+1, 3*M[i].col+1);
#if 3 == DIM
            BLAS_suscr_insert_entry(A, M[i].val, 3*M[i].row+2, 3*M[i].col+2);
#else
            BLAS_suscr_insert_entry(A, 0.0     , 3*M[i].row+2, 3*M[i].col+2);
#endif
        }

        BLAS_suscr_end(A);
    }

    template<>
    template<typename Ti, typename Tq>
    void SparseMatrix<double>::Fill_from_Coo_blk_id_3(const CoordMatrix<Ti, Tq>& M)
    {
        for(size_t i = 0; i < M.size(); ++i)
        {
            CHECK_DEBUG((int )3*M[i].row+2 >= 0, 0);
            CHECK_DEBUG((int )3*M[i].col+2 >= 0, 0);

            BLAS_duscr_insert_entry(A, M[i].val, 3*M[i].row+0, 3*M[i].col+0);
            BLAS_duscr_insert_entry(A, M[i].val, 3*M[i].row+1, 3*M[i].col+1);
#if 3 == DIM
            BLAS_duscr_insert_entry(A, M[i].val, 3*M[i].row+2, 3*M[i].col+2);
#else
            BLAS_duscr_insert_entry(A, 0.0     , 3*M[i].row+2, 3*M[i].col+2);
#endif
        }

        BLAS_duscr_end(A);
    }

    template void SparseMatrix<float >::Fill_from_Coo_blk_id_3(const CoordMatrix<int64, float >& M);
    template void SparseMatrix<double>::Fill_from_Coo_blk_id_3(const CoordMatrix<int64, double>& M);

    template<typename Tp>
    void SparseMatrix<Tp>::Report_stats(const char* name)
    {
        /// TODO Implemented Report_stats
        Log::Instance().write("WARN: Function %s(name = \"%s\") is not implemented when using SpBLAS.\n", __FUNCTION__, name);
    }

    template void SparseMatrix<float >::Report_stats(const char*);
    template void SparseMatrix<double>::Report_stats(const char*);


}

