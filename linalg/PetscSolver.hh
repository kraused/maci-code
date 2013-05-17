
// vi: tabstop=4:expandtab

#ifndef MACI_LINALG_PETSC_SOVLER_HH_INCLUDED
#define MACI_LINALG_PETSC_SOVLER_HH_INCLUDED 1

#ifdef HAVE_CONFIG_HH
#include "config.hh"
#endif

#include "Assembler.hh"
#include "mpi/Communicator.hh"
#include "linalg/Solver.hh"

#ifdef HAVE_PETSC
#include "petsc.h"
#include "petscvec.h"
#include "petscmat.h"
#include "petscksp.h"
#endif

namespace linalg {

    /// Trilinos solver for parallel solver
    class PetscSolver : public Solver
    {

    public:
        /// Create a solver
        /// @param ass      the assembler
        /// @param comm     the communicator to use
        /// @param MaxIter  maximal number of iterations
        /// @param Tol      tolerance to reach
        PetscSolver(Assembler *ass, const mpi::Communicator& c,
                int MaxIter, double Tol);

        /// Destroy the solver
        ~PetscSolver();

        /// @overrides
        void solve(double *x, double *y, int n);

    private:
        /// Number of rows
        int nrows;
        /// The solver communicator
        mpi::Communicator comm;
        /// Distributed dense vectors which contain the solution and
        /// the right hand side resp.
        Vec vecX, vecB;
        /// The matrix
        Mat mat;
        /// Krylov subspace solver (conjugated gradient)
        KSP solver;
        /// Preconditioner
        PC  pc;
        /// Maximal number of iterators
        int maxiter;
        /// Tolerance to reach
        double TOL;

        /// Construct the instance 
        /// This is a seperate routine since this simplifies debugging
        /// with gdb as there is a bug in gcc which prevents local
        /// variables to be shown in the debugger
        /// See the class constructor for a description of the arguments
        void construct(Assembler *ass, const mpi::Communicator& c,
                   int MaxIter, double Tol);
    };

}

#endif

