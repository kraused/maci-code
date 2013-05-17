
// vi: tabstop=4:expandtab

#ifndef MACI_LINALG_SPARSE_MATRIX_HH_INCLUDED
#define MACI_LINALG_SPARSE_MATRIX_HH_INCLUDED 1

#ifdef HAVE_CONFIG_HH
#include <config.hh>
#endif

/// Sparse matrix implementation
///
/// The template class SparseMatrix<Float> is implemented
/// using different packages. The choice of the sparse linear
/// algebra format is done at compile time.
///
/// All SparseMatrix<Float> implementations provide the same
/// interface:
///
/// The enum type SpMV_Op is defined and can take the values
/// SPMV_OP_NORMAL and SPMV_OP_TRANS. The SparseMatrix class
/// has a constructor
///
///     SparseMatrix(int numberRows, int numberCols)
///
/// It provides NumRows() and NumCols() functions to query
/// the matrix size. Using the function
///
///     template<typename Ti, typename Tq>
///     void Fill_from_Coo_3x(const linalg::CoordMatrix<Ti,Tq>&);
///
/// a convenient interface is provided to create a matrix
/// from a coordinate matrix. Here, each scalar entry is duplicated
/// on the diagonal of a 3x3 block.
/// The function
///
///     void SpMV(SpMV_Op, Tp alpha, Tp* x, Tp beta, Tp* y)
///
/// performs the sparse matrix-vector multiplication y = alpha*A*x + beta*y 

#if   defined(HAVE_MKL)
#include "linalg/SparseMKLMatrix.hh"
#elif defined(HAVE_OSKI)
#include "linalg/SparseOSKIMatrix.hh"
#elif defined(HAVE_SPBLAS)
#include "linalg/SparseBLASMatrix.hh"
#else
#include "linalg/SparseCSRMatrix.hh"
#endif

#endif

