
// vi: tabstop=4:expandtab

#include "linalg/PetscSolver.hh"

#include "TauProfiling.hh"
#include "DesignByContract.hh"

using namespace linalg;

/// Insert a row
/// @warning Overwrites the input val
static void insertVals(Mat *mat, int p, int k, PetscScalar *vals, int *inds)
{
    int q = 0;

    for(int j = 0; j < k; ++j) {
        inds[j] *= 3;
    }
    q = 3*p;
    MatSetValues(*mat, 1, &q, k, inds, vals, INSERT_VALUES);

    for(int j = 0; j < k; ++j) {
        inds[j] += 1;
    }
    q = 3*p + 1;
    MatSetValues(*mat, 1, &q, k, inds, vals, INSERT_VALUES);

    for(int j = 0; j < k; ++j) {
        inds[j] += 1;
    }
    q = 3*p + 2;
    MatSetValues(*mat, 1, &q, k, inds, vals, INSERT_VALUES);
}

/// Insert matrix values in the distributed sparse CRS matrix
static void insert(AssemblerOutputMatrix& A, Mat *mat)
{
    CHECK_PRECOND_ALWAYS(mat);
    
    int n = A.maxBandwidth();
    CHECK_ALWAYS(n > 0, "matrix is empty?");

    PetscScalar *vals = new PetscScalar[n];
    int         *inds = new int[n];

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
    
    MatAssemblyBegin(*mat, MAT_FINAL_ASSEMBLY); 
    MatAssemblyEnd  (*mat, MAT_FINAL_ASSEMBLY);

    delete[] vals;
    delete[] inds;
}

PetscSolver::PetscSolver(Assembler *ass, const mpi::Communicator& c, int MaxIter, double Tol)
: nrows(0), comm(c), vecX(0), vecB(0), mat(0), maxiter(MaxIter), TOL(Tol)
{
    construct(ass, c, MaxIter, Tol);
}

void PetscSolver::construct(Assembler *ass, const mpi::Communicator& c, int MaxIter, double Tol)
{
    AssemblerOutputMatrix& A = ass->matrix;
    if(0 == A.size()) 
    {
        CHECK_DEBUG(0 == comm.size(), "wrong communicator argument");
        return;
    }

    nrows = 3*A.getRelativeNumOfRows();

    PETSC_COMM_WORLD = comm.getHandle();
    PetscInitialize(0, 0, 0, 0);

    /// TODO Should we compute the DIAGONAL and OFF-DIAGONAL blocks in
    ///  advance?
    int n = A.maxBandwidth();
    MatCreateMPIAIJ(PETSC_COMM_WORLD, 
            nrows, nrows, 
            PETSC_DETERMINE, PETSC_DETERMINE, 
            n, PETSC_NULL, 
            n, PETSC_NULL, &mat);
    insert(A, &mat);

//  MatGetVecs(mat, &vecX, &vecB);
    VecCreateMPI(PETSC_COMM_WORLD, nrows, PETSC_DETERMINE, &vecX);
    VecCreateMPI(PETSC_COMM_WORLD, nrows, PETSC_DETERMINE, &vecB);

    PetscViewer viewer;
    PetscViewerBinaryOpen(PETSC_COMM_WORLD, "Lambda.mat", FILE_MODE_WRITE, &viewer);
    MatView(mat, viewer);
    PetscViewerDestroy(viewer);

#ifndef NDEBUG
    {
        PetscInt i, j;
        
        MatGetSize(mat, &i, &j);
        CHECK_DEBUG(i == j, "global sizes in PetSC do not match");

        MatGetLocalSize(mat, &i, &j);
        CHECK_DEBUG(i == 3*(PetscInt )A.getRelativeNumOfRows(), "local sizes in PetSC do not match");
        CHECK_DEBUG(j == 3*(PetscInt )A.getRelativeNumOfRows(), "local sizes in PetSC do not match");

        VecGetSize(vecX, &i);
        VecGetSize(vecB, &j);
        CHECK_DEBUG(i == j, "global sizes in PetSC do not match");

        VecGetLocalSize(vecX, &i);
        CHECK_DEBUG(i == nrows, "local size is not equal to the row size");

        VecGetLocalSize(vecB, &i);
        CHECK_DEBUG(i == nrows, "local size is not equal to the row size");
    }
#endif
    
    KSPCreate(PETSC_COMM_WORLD, &solver);
    KSPSetType               (solver, KSPCG);
    KSPSetOperators          (solver, mat, mat, SAME_PRECONDITIONER);
    KSPSetTolerances         (solver, 1e-20, TOL, PETSC_DEFAULT, MaxIter);
/// Since we use the same multiplier vector for the velocities and the displacements
/// we cannot use non-zero initial guesses
/// KSPSetInitialGuessNonzero(solver, PETSC_TRUE);

    KSPGetPC(solver, &pc);
    PCSetType(pc, PCJACOBI);

    KSPSetUp(solver);
}

PetscSolver::~PetscSolver()
{
    if(0 == comm.size()) {
        return;
    }

    KSPDestroy(solver);
    VecDestroy(vecX);
    VecDestroy(vecB);
    MatDestroy(mat);

    PetscFinalize();
}

/// Copy from array to Petsc vector
void copy(int n, const double x[], Vec vec)
{
    PetscScalar *v;
    VecGetArray(vec, &v);

    for(int i = 0; i < n; ++i) {
        v[i] = x[i];
    }

    VecRestoreArray(vec, &v);
}

/// Copy from Petsc vector to array
void copy(Vec vec, int n, double x[])
{
    PetscScalar *v;
    VecGetArray(vec, &v);

    for(int i = 0; i < n; ++i) {
        x[i] = v[i];
    }

    VecRestoreArray(vec, &v);
}

void PetscSolver::solve(double *x, double *y, int n)
{
    TAU_PROFILE("PetscSolver::solve", "", TAU_DEFAULT);

    CHECK_PRECOND_ALWAYS(n == nrows);
    CHECK_PRECOND_DEBUG(x);
    CHECK_PRECOND_DEBUG(y);

    copy(n, x, vecB);
/// KSPSetInitialGuessNonzero not set
/// copy(n, y, vecX);

#ifndef NDEBUG
    KSPMonitorSet(solver, KSPMonitorTrueResidualNorm, PETSC_NULL, PETSC_NULL);
#endif

    KSPSolve(solver, vecB, vecX);

    KSPConvergedReason reason;
    KSPGetConvergedReason(solver, &reason);

#ifndef NDEBUG
    PetscInt its;
    KSPGetIterationNumber(solver, &its);

    PetscReal rnorm;
    KSPGetResidualNorm(solver, &rnorm);

//  KSPView(solver, PETSC_VIEWER_STDOUT_WORLD);

    if(0 == comm.myrank())
        printf(" PetscSolver::solve: Number of iters = %d; Final residual = %e\n", its, rnorm);
#endif

    CHECK_ALWAYS(reason > 0, "The linear solver failed");

    copy(vecX, n, y);

#ifndef NDEBUG
    comm.barrier();
#endif
}

