//
// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#if 2 == DIM
%module maciS2
#else
%module maciS3
#endif
%{
#include "config.hh"
#include "ParallelMultiplierMatrix.hh"
%}

%include "ParallelMultiplierMatrix.hh"

#ifdef HAVE_TRILINOS
%template(ParallelTrilinosMultiplierMatrix) ParallelMultiplierMatrix<linalg::TrilinosSolver>;
#endif
#ifdef HAVE_PETSC
%template(ParallelPetscMultiplierMatrix)    ParallelMultiplierMatrix<linalg::PetscSolver>;
#endif

