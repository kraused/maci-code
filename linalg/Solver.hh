
// vi: tabstop=4:expandtab

#ifndef MACI_LINALG_SOLVER_HH_INCLUDED
#define MACI_LINALG_SOLVER_HH_INCLUDED

namespace linalg {

    /// Abstract base class for all linear algebra solver
    class Solver
    {

    public:
        /// The solver routine
        /// @param x    local part of the input vector
        /// @param y    local output vector
        /// @param n    length of the vectors x and y
        virtual void solve(double *x, double *y, int n) = 0;

        /// Shut up
        virtual ~Solver()
        { }
        
    };

}

#endif

