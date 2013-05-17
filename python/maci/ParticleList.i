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
#include "ParticleList.hh"
#include "ParticleListSelector.hh"
%}

/* Get rid of warning 401: We cannot instanciate std::vector<md::interface::ParticleHandle> and
   hence we need to ignore the warning */
%warnfilter(401) ParticleList;

%typemap(in) (const polylib::Box<double,DIM>&)
{
	double lower[3], upper[3];

	/* Input is a list of six corners */
	if(PyList_Check($input)) {
		for(int d = 0; d < DIM; ++d) {
			lower[d] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem($input,  d)));
			upper[d] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem($input,3+d)));
		}
		
		$1 = new polylib::Box<double,DIM>(lower, upper);
	} else {
		PyErr_SetString(PyExc_TypeError, "not a list");
		return 0;
	}
}

%typemap(freearg) (const polylib::Box<double,DIM>&)
{
	delete $1;
}

%include "ParticleList.hh"
%include "ParticleListSelector.hh"

%template(AddSelectionHandshake) ParticleList::AddSelection<SelectHandshakeParticles>;
%template(AddSelectionAll) ParticleList::AddSelection<SelectAllParticles>;
%template(AddSelectionNonHandshake) ParticleList::AddSelection<SelectNonHandshakeParticles>;
%template(AddSelectionInBox) ParticleList::AddSelection<SelectParticlesInBox>;

%extend ParticleList 
{

	int size()
	{
		return $self->size();
	}

};

