
// vi: tabstop=4:expandtab

#ifndef MACI_LINALG_SPARSE_OSKI_MATRIX_HH_INCLUDED
#define MACI_LINALG_SPARSE_OSKI_MATRIX_HH_INCLUDED 1

extern "C"
{
#include <oski/oski_Tid.h>
// #include <oski/oski_Tis.h>
}

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

    template<typename Tp>
    struct OskiTypes
    {
        typedef void*   Matrix;
        typedef void*   Vec;
    };

//  template<>
//  struct OskiTypes<float>
//  {
//      typedef oski_matrix_t_Tis   Matrix;
//      typedef oski_vecview_t_Tis  Vec;
//  };

    template<>
    struct OskiTypes<double>
    {
        typedef oski_matrix_t_Tid   Matrix;
        typedef oski_vecview_t_Tid  Vec;
    };


    /// Sparse Matrix based on the Berkeley OSKI library
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
        /// Number of nonzeroes
        int nnz;

        /// CSR representation of the matrix
        int* ap;
        int* ai;
        Tp*  av;
        
        /// OSKI matrix object
        typename OskiTypes<Tp>::Matrix A;
        /// Vector objects used to implement SpMV
        typename OskiTypes<Tp>::Vec X, Y;
    };

}

#endif

