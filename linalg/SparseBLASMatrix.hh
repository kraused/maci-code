
// vi: tabstop=4:expandtab

#ifndef MACI_LINALG_SPARSE_BLAS_MATRIX_HH_INCLUDED
#define MACI_LINALG_SPARSE_BLAS_MATRIX_HH_INCLUDED 1

#include "blas_sparse.h"

namespace linalg 
{

    template<typename Ti, typename Tq>
    class CoordMatrix;

    /// Supported SpMV types
    enum SpMV_Op
    {
        SPMV_OP_NORMAL = 0,     ///< Multiply with the matrix
        SPMV_OP_TRANS  = 1      ///< Multiply with the transposed matrix
    };


    /// Sparse Matrix based on the Sparse BLAS Interface
    ///
    /// Unfortunately Sparse BLAS is not implemented by many packages
    /// and currently we can only use the rather slow NIST reference
    /// implementation
    template<typename Tp>
    class SparseMatrix
    {
    
    public:
        /// Create a new instance. The first argument equals the
        /// number of rows. The second argument equals the number of
        /// columns.
        SparseMatrix(int M, int N);

        /// Destructor
        ~SparseMatrix();

        /// Copy the entries from a coordinate matrix (without duplicated
        /// entries) and inflate each entry of the coordinate matrix to a
        /// 3x3 block equal to the value times identity. This is useful since
        /// many matrices have the same values for each dimension and hence
        /// we assemble the block value only once
        template<typename Ti, typename Tq>
        void Fill_from_Coo_blk_id_3(const CoordMatrix<Ti, Tq>& A);

        /// Matrix vector muliplication
        void SpMV(SpMV_Op op, Tp alpha, Tp* x, Tp beta, Tp* b);

        /// Print a report with statistics about the matrix
        void Report_stats(const char* name);

        /// Return the number of rows
        inline int NumRows() const
        {
            return nrows;
        }

        /// Return the number of columns
        inline int NumCols() const
        {
            return ncols;
        }

    private:
        /// Number of rows
        int nrows;
        /// Number of columns
        int ncols;
        /// Opaque sparse matrix object
        blas_sparse_matrix A;
    };

}

#endif

