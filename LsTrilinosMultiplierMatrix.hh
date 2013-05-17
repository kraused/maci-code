
// vi: tabstop=4:expandtab

#ifndef MACI_LS_TRILINOS_MULTIPLIER_MATRIX_HH_INCLUDED
#define MACI_LS_TRILINOS_MULTIPLIER_MATRIX_HH_INCLUDED 1

#include "LsParallelMultiplierMatrix.hh"
#include "linalg/TrilinosSolver.hh"

typedef LsParallelMultiplierMatrix<linalg::TrilinosSolver> LsTrilinosMultiplierMatrix;

#endif


