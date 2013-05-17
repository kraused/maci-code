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
#include "Function.hh"
#include "python/maci/FunctionPyObj.hh"
%}

%include "Function.hh"

%template(RealFunction3) Function<double,3>;
%template(IntFunction3)  Function<int   ,3>;

%include "python/maci/FunctionPyObj.hh"

%template(RealFunctionPyObj3) FunctionPyObj<double,3>;
%template(IntFunctionPyObj3)  FunctionPyObj<int   ,3>;

