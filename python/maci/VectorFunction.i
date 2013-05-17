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
#include "VectorFunction.hh"
#include "python/maci/VectorFunctionPyObj.hh"
%}

%include "VectorFunction.hh"

%template(RealVectorFunction33) VectorFunction<double,3,3>;
%template(RealVectorFunction43) VectorFunction<double,4,3>;

%include "python/maci/VectorFunctionPyObj.hh"

%template(RealVectorFunctionPyObj33) VectorFunctionPyObj<double,3,3>;
%template(RealVectorFunctionPyObj43) VectorFunctionPyObj<double,4,3>;

