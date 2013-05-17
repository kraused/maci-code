
// vi: tabstop=4:expandtab

#ifndef MACI_MULTIPLIER_MATRIX_HH_INCLUDED
#define MACI_MULTIPLIER_MATRIX_HH_INCLUDED

#include "Pointers.hh"
#include "AssemblyProduct.hh"
#include "linalg/Solver.hh"

/// Forwarding
class Maci;


/// MultiplierMatrix: Base class for all multiplier matrices
class MultiplierMatrix : public Pointers, public AssemblyProduct
{

public:
    /// Default constructor
    /// @param[in]  app The Maci application instance
    MultiplierMatrix(Maci* app);

    /// Destroy the instance
    ~MultiplierMatrix();

    /// Invert the multiplier matrix: Solve for the vector b
    virtual void solve(double x[], double b[], long n)
    {
        CHECK_PRECOND_DEBUG(solver);
        solver->solve(x, b, n);
    }

protected:
    /// The solver
    linalg::Solver* solver;
};

#endif

