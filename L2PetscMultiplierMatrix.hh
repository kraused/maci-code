
// vi: tabstop=4:expandtab

#ifndef MACI_L2_PETSC_MULTIPLIER_MATRIX_HH_INCLUDED
#define MACI_L2_PETSC_MULTIPLIER_MATRIX_HH_INCLUDED 1

#include "L2ParallelMultiplierMatrix.hh"
#include "linalg/PetscSolver.hh"

typedef L2ParallelMultiplierMatrix<linalg::PetscSolver> L2PetscMultiplierMatrix;

#endif

