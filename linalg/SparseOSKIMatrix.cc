
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

    namespace
    {
        struct OskiInitializer
        {

            /// Initialize the OSKI library when x is created
            /// which is the case when the shared library is
            /// loaded
            OskiInitializer()
            {
                oski_Init();
            }

            /// shut down OSKI when x runs out of scope
            ~OskiInitializer()
            {
                oski_Close();
            }

        } x;
    }

    /*
    template<>
    SparseMatrix<float>::SparseMatrix(int M, int N)
    : nrows(M), ncols(N)
    {
    }
    */

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

    /*
    template<>
    void SparseMatrix<float>::SpMV(SpMV_Op op, float alpha, float x[], float y[])
    {
    }
    */

    template<>
    void SparseMatrix<double>::SpMV(SpMV_Op op, double alpha, double* x, double beta, double* y)
    {
        TAU_PROFILE("SparseMatrix<double>::SpMV", "", TAU_DEFAULT);

        switch(op)
        {
        case SPMV_OP_NORMAL:
            X = oski_CreateVecView_Tid(x, ncols, STRIDE_UNIT);
            Y = oski_CreateVecView_Tid(y, nrows, STRIDE_UNIT);

            oski_MatMult_Tid(A, OP_NORMAL, alpha, X, beta, Y);
            break;
        case SPMV_OP_TRANS:
            X = oski_CreateVecView_Tid(x, nrows, STRIDE_UNIT);
            Y = oski_CreateVecView_Tid(y, ncols, STRIDE_UNIT);

            oski_MatMult_Tid(A, OP_TRANS , alpha, X, beta, Y);
            break;
        }

        oski_DestroyVecView_Tid(X);
        oski_DestroyVecView_Tid(Y);

//      /* Here we use the implicit profiling capabilities of OSKI. From time to time
//         we call the tuning library call which uses data gathered from the previous
//         MatMul calls to do optimizations */
//      static int ncalls = 0;
//      if(0 == (++ncalls)%10)
//      {
//          oski_TuneMat_Tid(A);
//#ifndef NDEBUG
//          printf(" [%3d]: --- Transformations: \"%s\" ---\n", gcpe()->world.myrank(), oski_GetMatTransforms(A));
//#endif
//      }
    }

    /*
    template<>
    template<typename Ti, typename Tq>
    void SparseMatrix<float>::Fill_from_Coo_blk_id_3(const CoordMatrix<Ti, Tq>& M)
    {
    }

    template
    void SparseMatrix<float>::Fill_from_Coo_blk_id_3(const CoordMatrix<int64, double>& M);
    */

    template<>
    template<typename Ti, typename Tq>
    void SparseMatrix<double>::Fill_from_Coo_blk_id_3(const CoordMatrix<Ti, Tq>& M)
    {
        Coo_blk_id_3_to_CSR(M, nrows, ncols, &nnz, &ap, &ai, &av);
        A = oski_CreateMatCSR_Tid(ap, ai, av, nrows, ncols, SHARE_INPUTMAT, 2, INDEX_ZERO_BASED, INDEX_UNIQUE);
    }

    template
    void SparseMatrix<double>::Fill_from_Coo_blk_id_3(const CoordMatrix<int64, double>& M);

    template<typename Tp>
    void SparseMatrix<Tp>::Report_stats(const char* name)
    {
        CSR_Report(name, nrows, ncols, nnz, ap, ai, av);
    }

    template
    void SparseMatrix<float>::Report_stats(const char* name);
    template
    void SparseMatrix<double>::Report_stats(const char* name);

}

