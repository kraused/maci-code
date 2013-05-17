
// vi: tabstop=4:expandtab

#ifndef MACI_PARALLEL_MAT_MUL_HH_INCLUDED
#define MACI_PARALLEL_MAT_MUL_HH_INCLUDED 1

#include "linalg/CoordMatrix.hh"
#include "Int64.hh"
#include "mpi/Communicator.hh"


/// Perform a paralle matrix-matrix multiplication. The
/// function computes C = beta*C + alpha*A*B. The function
/// is collective on world. The input matrices A and B
/// are guaranteed not be modified except for sorting.
/// The input distribution of A and B is arbitrary but the
/// output matrix is distributed by rows such that all the
/// entries (p | ...) are stored on processing element p.
/// The function assumes that the matrices A and B conform
/// to the standard AssemblerOutputMatrix type.
void ParallelMatMul(double alpha, 
                    linalg::CoordMatrix<int64, double>& A,
                    linalg::CoordMatrix<int64, double>& B,
                    double beta,
                    linalg::CoordMatrix<int64, double>* C);
                    
#endif

