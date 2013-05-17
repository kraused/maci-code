
// vi: tabstop=4:expandtab

#ifndef MACI_L2_TRILINOS_MULTIPLIER_MATRIX_HH_INCLUDED
#define MACI_L2_TRILINOS_MULTIPLIER_MATRIX_HH_INCLUDED 1

#include "L2ParallelMultiplierMatrix.hh"
#include "linalg/TrilinosSolver.hh"

typedef L2ParallelMultiplierMatrix<linalg::TrilinosSolver> L2TrilinosMultiplierMatrix;

#endif


