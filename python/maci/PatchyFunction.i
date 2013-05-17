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
#include "PatchyFunction.hh"
%}

%typemap(in) (const double*)
{
	if(PyList_Check($input)) {
		ASSERT(3 == PyList_Size($input));
		$1 = new double[3];
		for(int d = 0; d < 3; ++d) {
			$1[d] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem($input,d)));

		}
	} else {
		PyErr_SetString(PyExc_TypeError, "not a list");
		return 0;
	}
}

%typemap(freearg) (const double*)
{
	delete[] $1;
}

%include "PatchyFunction.hh"

