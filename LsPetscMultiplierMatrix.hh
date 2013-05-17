
// vi: tabstop=4:expandtab

#ifndef MACI_LS_PETSC_MULTIPLIER_MATRIX_HH_INCLUDED
#define MACI_LS_PETSC_MULTIPLIER_MATRIX_HH_INCLUDED 1

#include "LsParallelMultiplierMatrix.hh"
#include "linalg/PetscSolver.hh"

typedef LsParallelMultiplierMatrix<linalg::PetscSolver> LsPetscMultiplierMatrix;

#endif


