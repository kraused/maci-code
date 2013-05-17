
// vi: tabstop=4:expandtab

#ifndef MACI_MESH_MASS_MATRIX_HH_INCLUDED
#define MACI_MESH_MASS_MATRIX_HH_INCLUDED 1

#include "AssemblyProduct.hh"
#include "linalg/SparseMatrix.hh"


/// MeshMassMatrix: Base class for all Finite Element
///                 mass matrices
class MeshMassMatrix : public AssemblyProduct
{

public:
    MeshMassMatrix();

    /// Delete the matrix A if it is not null
    ~MeshMassMatrix();
        
    /// MeshMassMatrix mimics the interface of SparseMatrix<double>. Note
    /// that we cannot derive MeshMassMatrix from linalg::SparseMatrix<double>
    /// because the number of rows and columns is not known at the time of
    /// creation
    int NumRows() const;
    int NumCols() const;

    void SpMV(linalg::SpMV_Op op, double alpha, double* x, double  beta, double* y);

private:
    /// The matrix representation. We use a
    /// generic sparse matrix even if the matrix
    /// is lumped. In this case performance is
    /// no concern anyway
    linalg::SparseMatrix<double>* A;

protected:
    /// Copy the values from the output of the
    /// primary assembler
    void CreateMatrix();
};

#endif

