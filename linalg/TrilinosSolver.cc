
// vi: tabstop=4:expandtab

#include "linalg/TrilinosSolver.hh"

#include "DesignByContract.hh"
#include "TauProfiling.hh"
#include "Delete.hh"

#include "Epetra_MpiComm.h"
#include "Epetra_Map.h"
#include "Epetra_Vector.h"
#include "Epetra_CrsMatrix.h"
#include "Epetra_LinearProblem.h"
#include "Epetra_Import.h"
#include "AztecOO.h"
#include "EpetraExt_RowMatrixOut.h"

using namespace linalg;

/// Insert a row
/// @warning Overwrites the input val
static void insertVals(Epetra_CrsMatrix *mat, int p, int k, double *vals, int *inds)
{
    int err = 0;

    for(int j = 0; j < k; ++j) {
        inds[j] *= 3;
    }
    err += mat->InsertGlobalValues(3*p  ,k,vals,inds);

    for(int j = 0; j < k; ++j) {
        inds[j] += 1;
    }
    err += mat->InsertGlobalValues(3*p+1,k,vals,inds);

    for(int j = 0; j < k; ++j) {
        inds[j] += 1;
    }
    err += mat->InsertGlobalValues(3*p+2,k,vals,inds);

    CHECK_POSTCOND_ALWAYS(0 == err);
}

/// Insert matrix values in the distributed sparse CRS matrix
static void insert(AssemblerOutputMatrix& A, Epetra_CrsMatrix *mat)
{
    CHECK_PRECOND_ALWAYS(mat);
    
    int n = A.maxBandwidth();
    CHECK_ALWAYS(n > 0, "matrix is empty?");

    double *vals = new double[n];
    int    *inds = new int[n];

    A.sort();

    int p = A[0].row;
    int k = 0;
    std::size_t i = 0;
    while(i < A.size()) {
        if(A[i].row == p) {
            CHECK_DEBUG(k >= 0 && k < n, 0);
            vals[k] = A[i].val;
            inds[k] = A[i].col;
            ++k;
            ++i;
        } else {
            insertVals(mat, p, k, vals, inds);

            p = A[i].row;
            k = 0;
        }
    }
    insertVals(mat, p, k, vals, inds);
    
    mat->FillComplete();

    delete[] vals;
    delete[] inds;
}

TrilinosSolver::TrilinosSolver(Assembler *ass, const mpi::Communicator& c, int MaxIter, double Tol)
: nrows(0), comm(c), communicator(0), map(0), vecX(0), vecB(0), mat(0),
  problem(0), solver(0), maxiter(MaxIter), TOL(Tol)
{
    construct(ass, c, MaxIter, Tol);
}

void TrilinosSolver::construct(Assembler *ass, const mpi::Communicator& c, int MaxIter, double Tol)
{
    AssemblerOutputMatrix& A = ass->matrix;
    if(0 == A.size())
        return;

    nrows = 3*A.getRelativeNumOfRows();

    communicator = new Epetra_MpiComm(comm.getHandle());
    CHECK_ALWAYS(comm.myrank() == communicator->MyPID(),
        "Epetra_MpiComm reorders the processes :(");

    map  = new Epetra_Map(-1, nrows, 0, *communicator);

#ifndef NDEBUG
    std::cout << (*map);
#endif

    vecX = new Epetra_Vector(*map);
    vecB = new Epetra_Vector(*map);

    mat  = new Epetra_CrsMatrix(Copy,*map,A.maxBandwidth());
    insert(A, mat);

    EpetraExt::RowMatrixToMatrixMarketFile("Lambda.mm", *mat, "multiplier matrix", "L2 multiplier matrix");

    problem = new Epetra_LinearProblem(mat, vecX, vecB);
    solver  = new AztecOO(*problem);

    solver->SetAztecOption(AZ_solver , AZ_cg);
    solver->SetAztecOption(AZ_precond, AZ_Jacobi);
#ifdef NDEBUG
    solver->SetAztecOption(AZ_output , AZ_warnings);
#endif

    CHECK_POSTCOND_DEBUG(communicator);
    CHECK_POSTCOND_DEBUG(map);
}

TrilinosSolver::~TrilinosSolver()
{
    DeletePointer(solver);
    DeletePointer(problem);
    DeletePointer(mat);
    DeletePointer(vecB);
    DeletePointer(vecX);
    DeletePointer(map);
    DeletePointer(communicator);
}

void TrilinosSolver::solve(double *x, double *y, int n)
{
    TAU_PROFILE("TrilinosSolver::solve", "", TAU_DEFAULT);

    CHECK_PRECOND_ALWAYS(n == nrows);
    CHECK_PRECOND_DEBUG(x);
    CHECK_PRECOND_DEBUG(y);
    CHECK_PRECOND_DEBUG(vecB);
    CHECK_PRECOND_DEBUG(vecX);
    CHECK_PRECOND_DEBUG(mat);
    CHECK_PRECOND_DEBUG(communicator);

#ifndef NDEBUG
    communicator->Barrier();
#endif

    for(int i = 0; i < n; ++i) {
        (*vecB)[i] = x[i];
        (*vecX)[i] = y[i];
    }

    solver->Iterate(maxiter,TOL);

#ifndef NDEBUG
    if(0 == communicator->MyPID()) {
        printf(" TrilinosSolver::solve: %d %e\n", solver->NumIters(), solver->TrueResidual());
    }

//  std::cout << (*vecX) << std::endl;
#endif

    for(int i = 0; i < n; ++i) {
        y[i] = (*vecX)[i];
    }

#ifndef NDEBUG
    communicator->Barrier();
#endif
}

