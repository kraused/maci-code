
// vi: tabstop=4:expandtab

#ifndef MACI_TRANSFER_OPERATOR_HH_INCLUDED
#define MACI_TRANSFER_OPERATOR_HH_INCLUDED

#include <valarray>

#include "AssemblyProduct.hh"
#include "Assembler.hh"
#include "Pointers.hh"

#include "linalg/SparseMatrix.hh"

/// Forwarding
class Maci;


/// TransferOperator: A transfer operator which transfers values
///                   from particle to mesh processing elements
class TransferOperator : public AssemblyProduct, public Pointers
{

public:
    /// Create a new instance
    /// @param[in]  app pointer to the Maci application
    TransferOperator(Maci* app);

    /// Delete the matrix
    ~TransferOperator();

    /// The function returns true if the matrix A is not-null. This allows
    /// for identifying the processing elements which are "worker" for
    /// the mexico jobs
    bool IsNonNull() const;

    /// TransferOperator mimics the interface of SparseMatrix<double>. Note
    /// that we cannot derive TransferOperator from linalg::SparseMatrix<double>
    /// because the number of rows and columns is not known at the time of
    /// creation
    int NumRows() const;
    int NumCols() const;

    void SpMV(linalg::SpMV_Op op, double alpha, double* x, double beta, double* b);

protected:
    /// Create the matrix
    void CreateMatrix();

    /// The sparse matrix
    linalg::SparseMatrix<double> *A;

};

#endif

