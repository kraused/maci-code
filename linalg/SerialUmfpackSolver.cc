
// vi: tabstop=4:expandtab

#include "linalg/SerialUmfpackSolver.hh"
#include "DesignByContract.hh"

#include "umfpack.h"

using namespace linalg;

SerialUmfpackSolver::SerialUmfpackSolver(Assembler *ass)
: nnz(0), ccsp(0), ccsi(0), ccse(0), sym(0), num(0)
{
    typedef Assembler::OutputMatrixType Matrix;

    Matrix& mat = ass->matrix();
    if(0 == mat.size()) {
        return;
    }

    nrows = 3*mat.getRelativeNumOfRows();
    nnz   = 3*mat.size();

    int *cooi = new int[nnz];
    int *cooj = new int[nnz];
    double *cooe = new double[nnz];

    int i = 0;
    for(Matrix::Iterator k = mat.begin(); k != mat.end(); ++k, i += 3) {
        cooi[i  ] = 3*k->row;
        cooj[i  ] = 3*k->col; 
        cooe[i  ] =   k->val;
        cooi[i+1] = 3*k->row + 1;
        cooj[i+1] = 3*k->col + 1;
        cooe[i+1] =   k->val;
        cooi[i+2] = 3*k->row + 2; 
        cooj[i+2] = 3*k->col + 2; 
        cooe[i+2] =   k->val; 
    }
    CHECK_ALWAYS(nnz == i, 0);

    ccsp = new int[nrows+1];
    ccsi = new int[nnz];
    ccse = new double[nnz];
    
    double info[UMFPACK_INFO];

    // Transform coordinate format to CCS format
        int ret = umfpack_di_triplet_to_col(nrows,nrows,nnz,cooi,cooj,cooe,
                        ccsp, ccsi, ccse, 0);
    printf(" umfpack_di_triplet_to_col = %d\n",ret);

    // Compute symbolic factorization
    umfpack_di_symbolic(nrows,nrows,ccsp,ccsi,ccse,&sym,NULL,info);
        printf(" umfpack_di_symbolic       = %d\n",(int )info[UMFPACK_STATUS]);
    CHECK_ALWAYS(sym, 0);

    // Numeric factorization
    umfpack_di_numeric(ccsp,ccsi,ccse,sym,&num,NULL,info);
        printf(" umfpack_di_numeric        = %d\n",(int )info[UMFPACK_STATUS]);
    CHECK_ALWAYS(num, 0);
    
    delete[] cooi;
    delete[] cooj;
    delete[] cooe;
}

void SerialUmfpackSolver::solve(double *x, double *y, int n)
{
    CHECK_PRECOND_ALWAYS(n == nrows);
    CHECK_PRECOND_DEBUG(x);
    CHECK_PRECOND_DEBUG(y);
    CHECK_PRECOND_DEBUG(ccsp);
    CHECK_PRECOND_DEBUG(ccse);

    double info[UMFPACK_INFO];
    umfpack_di_solve(UMFPACK_A, ccsp, ccsi, ccse, y, x, num, 0, info);

    CHECK_ALWAYS(0 == (int )info[UMFPACK_STATUS],
        "umfpack_di_solve failed");
}

