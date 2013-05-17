
// vi: tabstop=4:expandtab

#ifndef MACI_SPMV_JOB_HH_INCLUDED
#define MACI_SPMV_JOB_HH_INCLUDED 1

#include "mexico/mexico.hpp"
#include "linalg/SparseMatrix.hh"


/// FineFluctuationFilterJob: The mexico Job instance used
///                           by FineFluctuationFilter
class FineFluctuationFilterJob : public mexico::Job
{

public:
    /// Constructor
    /// @param[in]  matrix  The sparse matrix
    FineFluctuationFilterJob(linalg::SparseMatrix<double>* matrix);

    /// Perform the SpMV
    void exec(void* i_buf, void* o_buf);

private:
    /// The local matrix block. The matrix is distributed
    /// by rows and each row is stored on the processing element
    /// which initially holds the corresponding atom.
    linalg::SparseMatrix<double>* matrix;

};

#endif

