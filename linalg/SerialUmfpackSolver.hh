
// vi: tabstop=4:expandtab

#ifndef MACI_LINALG_SERIAL_UMFPACK_SOLVER_HH_INCLUDED
#define MACI_LINALG_SERIAL_UMFPACK_SOLVER_HH_INCLUDED 1

#include "linalg/Solver.hh"
#include "Assembler.hh"

namespace linalg {

    /// UMFPACK solver for serial solution of linear systems
    class SerialUmfpackSolver : public Solver
    {

    public:
        /// Create a solver
        /// @param ass  the assembler
        SerialUmfpackSolver(Assembler *ass);

        /// @overrides
        void solve(double *x, double *y, int n);

    private:
        /// Number of rows
        int nrows;
        /// Number of nonzero entries
        int nnz;
        /// Row pointer
        int *ccsp;
        /// Indices
        int *ccsi;
        /// Entreis
        double *ccse;
        /// Internal memory for UMFPACK
        void *sym;
        void *num;
    };

}

#endif

