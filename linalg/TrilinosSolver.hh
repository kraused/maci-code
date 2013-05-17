
// vi: tabstop=4:expandtab

#ifndef MACI_LINALG_TRILINOS_SOVLER_HH_INCLUDED
#define MACI_LINALG_TRILINOS_SOVLER_HH_INCLUDED 1

#include "Assembler.hh"
#include "mpi/Communicator.hh"
#include "linalg/Solver.hh"

/// The Trilinos classes can only be forward declared because of name clashes
/// with UG
class Epetra_MpiComm;
class Epetra_Map;
class Epetra_Vector;
class Epetra_CrsMatrix;
class Epetra_LinearProblem;
class AztecOO;

namespace linalg 
{

    /// Trilinos solver for parallel solver
    class TrilinosSolver : public Solver
    {

    public:
        /// Create a solver
        /// @param ass      the assembler
        /// @param comm     the communicator to use
        /// @param MaxIter  maximal number of iterations
        /// @param Tol      tolerance to reach
        TrilinosSolver(Assembler *ass, const mpi::Communicator& c, int MaxIter, double Tol);

        /// Destroy the solver
        ~TrilinosSolver();

        /// @overrides
        void solve(double *x, double *y, int n);

    private:
        /// Number of rows
        int nrows;
        /// In version 9.0.3 of Trilinos, Epetra is not duplicating the
        /// communicator but just storing the handle. We therefore must
        /// guarantee that the handle is valid for the lifetime of the
        /// Epetra object
        mpi::Communicator comm;
        /// The epetra communicator
        Epetra_MpiComm *communicator;
        /// The distribution map
        Epetra_Map *map;
        /// Distributed dense vectors which contain the solution and
        /// the right hand side resp.
        Epetra_Vector *vecX, *vecB;
        /// The matrix
        Epetra_CrsMatrix *mat;
        /// Linear problem class
        Epetra_LinearProblem *problem;
        /// The solver
        AztecOO *solver;
        /// Maximal number of iterators
        int maxiter;
        /// Tolerance to reach
        double TOL;

        /// Construct the instance 
        /// This is a seperate routine since this simplifies debugging
        /// with gdb as there is a bug in gcc which prevents local
        /// variables to be shown in the debugger
        /// See the class constructor for a description of the arguments
        void construct(Assembler *ass, const mpi::Communicator& c, int MaxIter, double Tol);
    };

}

#endif

