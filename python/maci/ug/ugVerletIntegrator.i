//
// Copyright (c) 2010 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#if 2 == DIM
%module maciS2
#else
%module maciS3
#endif
%{
#include "fe/ug/src/ugVerletIntegrator.hh"
%}

%rename(UgVerletIntegrator) UG_IF::VerletIntegrator;

%include "fe/ug/src/ugVerletIntegrator.hh"

