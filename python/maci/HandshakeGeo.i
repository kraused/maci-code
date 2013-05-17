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
#include "HandshakeGeo.hh"
%}

%typemap(in) (const double* )
{
	if(PyList_Check($input)) {
		$1 = new double[3];
		for(int d = 0; d < DIM; ++d) {
			$1[d] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem($input,d)));
		}
	} else {
		PyErr_SetString(PyExc_TypeError, "not a list");
		return 0;
	}
}

%typemap(free) (const double* )
{
	delete $1;
}

%typemap(in) (const polylib::Box<double,DIM>&)
{
	double lower[3], upper[3];

	/* Input is a list of six corners */
	if(PyList_Check($input)) 
	{
		for(int d = 0; d < DIM; ++d) 
		{
			lower[d] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem($input,  d)));
			upper[d] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem($input,3+d)));
		}
		
		$1 = new polylib::Box<double,DIM>(lower, upper);
	} 
	else 
	{
		PyErr_SetString(PyExc_TypeError, "not a list");
		return 0;
	}
}

%typemap(freearg) (const polylib::Box<double,DIM>&)
{
        delete $1;
}

// We ignore the copy constructor since otherwise SWIG fails
%ignore Cuboid::Cuboid(const Cuboid& C);

// There is no default constructor for element pointers
%ignore Intersects(const ElementPtr e) const;

%include "HandshakeGeo.hh"

