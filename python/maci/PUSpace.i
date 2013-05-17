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
#include "PUSpace.hh"
%}

%typemap(in) (const float h[])
%{
	if(PyList_Check($input)) {
		$1 = new float[3];
		for(int d = 0; d < DIM; ++d) {
			$1[d] = PyFloat_AsDouble(PyNumber_Float(
					PyList_GetItem($input,d)));

		}
	} else {
		PyErr_SetString(PyExc_TypeError, "not a list");
		return 0;
	}
%}

%typemap(freearg) (const float h[])
%{
	delete[] $1;
%}

/* Get rid of warning 401 */
%warnfilter(401) PUSpace;

%include "PUSpace.hh"

