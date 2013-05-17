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
#include "L2TrilinosMultiplierMatrix.hh"
%}

%template(L2TrilinosMultiplierMatrix) L2ParallelMultiplierMatrix<linalg::TrilinosSolver>;

%include "L2TrilinosMultiplierMatrix.hh"

