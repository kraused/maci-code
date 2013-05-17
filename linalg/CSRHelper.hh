
// vi: tabstop=4:expandtab

#ifndef MACI_LINALG_CSR_HELPER_HH_INCLUDED
#define MACI_LINALG_CSR_HELPER_HH_INCLUDED 1

#include "Logging.hh"

namespace linalg 
{

    template<typename Ti, typename Tq>
    class CoordMatrix;

    /// Copy the entries from a coordinate matrix (without duplicated
    /// entries) and inflate each entry of the coordinate matrix to a
    /// 3x3 block equal to the value times identity. This is useful since
    /// many matrices have the same values for each dimension and hence
    /// we assemble the block value only once.
    /// @param[in]  A   The coordinate matrix
    /// @param[in]  nrows   The number of rows
    /// @param[in]  ncols   The number of columns
    /// @param[out] nnz The number of non-zero entries
    /// @param[out] ap  The row pointers
    /// @param[out] ai  The column pointers
    /// @param[out] av  The matrix entries
    template<typename Tp, typename Ti, typename Tq>
    void Coo_blk_id_3_to_CSR(const CoordMatrix<Ti, Tq>& A, int nrows, int ncols, int* nnz, int** ap, int** ai, Tp** av);

    /// Compute SpMV using a CSR matrix. This function computes y = alpha*A*x.
    /// @param[in]  ap      The row pointers
    /// @param[in]  ai      The column pointers
    /// @param[in]  av      The matrix entries
    template<typename Tp>
    void CSR_SpMV(int nrows, int ncols, int* ap, int* ai, Tp* av, Tp alpha, Tp* x, Tp beta, Tp* y)
    {
        int i, j;
        Tp  y0;

        for(i = 0; i < nrows; ++i)
        {
            y0 = 0;
            for(j = ap[i]; j < ap[i+1]; ++j)
                y0 += av[j]*x[ai[j]];
            y[i] = beta*y[i] + alpha*y0;
        }
    }

    /// Compute the transposed SpMV using a CSR matrix. This function computes y = alpha*A^T*x.
    /// @param[in]  ap      The row pointers
    /// @param[in]  ai      The column pointers
    /// @param[in]  av      The matrix entries
    template<typename Tp>
    void CSR_SpMV_Trans(int nrows, int ncols, int* ap, int* ai, Tp* av, Tp alpha, Tp* x, Tp beta, Tp* y)
    {
        int i, j;
       
        if(0.0 == beta)
        {
            for(i = 0; i < ncols; ++i)
                y[i] = 0.0;
        }
        if(1.0 == beta)
            ;   /// Do nothing
        else
        {
            for(i = 0; i < ncols; ++i)
                y[i] *= beta;
        }

        for(i = 0; i < nrows; ++i)
            for(j = ap[i]; j < ap[i+1]; ++j)
                y[ai[j]] += alpha*av[j]*x[i];
    }

    /// Print a report about the matrix
    template<typename Tp>
    void CSR_Report(const char* name, int nrows, int ncols, int nnz, int* ap, int* ai, Tp* av)
    {
        int   max_bw, min_bw, i;
        float mean_bw;

        Log::Instance().write("\n");
        Log::Instance().write("============================================\n");
        Log::Instance().write("REPORT FOR MATRIX \"%s\":\n", name);
        
        Log::Instance().write("nrows        : %d\n", nrows);
        Log::Instance().write("ncols        : %d\n", ncols);
        Log::Instance().write("nnz          : %d (%.2f%%)\n", nnz, nnz/((double )nrows*ncols));

        max_bw  = 0;
        min_bw  = nrows;
        mean_bw = 0;

        for(i = 0; i < nrows; ++i)
        {
            max_bw = std::max(max_bw, ap[i+1]-ap[i]);
            min_bw = std::min(min_bw, ap[i+1]-ap[i]);

            mean_bw += (ap[i+1]-ap[i]);
        }

        mean_bw /= (double )nrows;

        Log::Instance().write("max  bw      : %d\n", max_bw);
        Log::Instance().write("min  bw      : %d\n", min_bw);
        Log::Instance().write("mean bw      : %f\n", mean_bw);
        
        Log::Instance().write("============================================\n");
        Log::Instance().write("\n");
    }

}

#endif

