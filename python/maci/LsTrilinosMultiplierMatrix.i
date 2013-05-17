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
#include "LsTrilinosMultiplierMatrix.hh"
%}

%template(LsTrilinosMultiplierMatrix) LsParallelMultiplierMatrix<linalg::TrilinosSolver>;

%include "LsTrilinosMultiplierMatrix.hh"

