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
#include "md/tremolo/src/tremoloInstance.hh"
%}

/* -----------------------------------------------------------------------------
 * Typemap for processor input
 * -------------------------------------------------------------------------- */
%typemap(in) (const int processors[3])
{
	if(PyList_Check($input)) {
		$1 = new int[3];
		for(int d = 0; d < 3; ++d) {
			$1[d] = PyInt_AsLong(PyNumber_Int(PyList_GetItem($input,d)));
		}
	} else {
		PyErr_SetString(PyExc_TypeError, "not a list");
		return 0;
	}
}

%typemap(freearg) (const int processors[3])
{
	delete[] $1;
}

/* -----------------------------------------------------------------------------
 * Typemap for box input
 * -------------------------------------------------------------------------- */
%typemap(in) (const double box[2][3])
{
	typedef double Vec[3];

	if(PyList_Check($input)) {
		$1 = new Vec[2];
		for(int d = 0; d < 3; ++d) {
			$1[0][d] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem($input,  d)));
			$1[1][d] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem($input,3+d)));
		}
	} else {
		PyErr_SetString(PyExc_TypeError, "not a list");
		return 0;
	}
}

%typemap(freearg) (const double box[2][3])
{
	delete[] $1;
}

/* -----------------------------------------------------------------------------
 * Handling extforce input
 * -------------------------------------------------------------------------- */
%typemap(in) (const const double F[][3])
{
	typedef double Vec[3];

	if(PyList_Check($input)) {
		$1 = new Vec[PyList_Size($input)];
		for(int i = 0; i < PyList_Size($input); ++i) {
			PyObject *list = PyList_GetItem($input,i);
			for(int d = 0; d < 3; ++d) {
				$1[i][d] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem(list,d)));
			}
		}
	} else {
		PyErr_SetString(PyExc_TypeError, "not a list");
		return 0;
	}
}

%typemap(freearg) (const const double F[][3])
{
	delete[] $1;
}

/* -----------------------------------------------------------------------------
 * Handling of the particle structure on script level
 * -------------------------------------------------------------------------- */

struct Particle
{
};

%extend Particle
{
	double __getitem__(int i)
	{
		if(0 == i || 1 == i || 2 == i) {
			return self->x[i];
		}
		if(3 == i || 4 == i || 5 == i) {
			return self->x[i];
		}
		if(6 == i || 7 == i || 8 == i) {
			return self->u[i-6];
		}
		if(9 == i || 10 == i || 11 == i) {
			return self->F[i-9];
		}
		if(12 == i) {
			return self->ParDat->mass;
		}
		
		PyErr_SetString(PyExc_TypeError, "index of out range");
		return 0.;
	}
	
	void __setitem__(int i, double val)
	{
		if(0 == i || 1 == i || 2 == i) {
			self->x[i] = val;
			return;
		}
		if(3 == i || 4 == i || 5 == i) {
			self->x[i] = val;
			return;	
		}
		if(6 == i || 7 == i || 8 == i) {
			self->u[i-6] = val;
			return;
		}
		if(9 == i || 10 == i || 11 == i) {
			self->F[i-9] = val;
			return;
		}
		if(12 == i) {
			self->ParDat->mass = val;
			return;
		}
		
		PyErr_SetString(PyExc_TypeError, "index of out range");
	}
	
	char* __str__() {
		static char str[1024];
		snprintf(str, 1024, "{ x = [ %8.4f, %8.4f, %8.4f ],"
				    " x0 = [ %8.4f, %8.4f, %8.4f ],"
				     " u = [ %8.4f, %8.4f, %8.4f ],"
				     " F = [ %8.4f, %8.4f, %8.4f ],"
				     " m = %8.4f }",
				    self->x[0], self->x[1], self->x[2],
				    self->x[3], self->x[4], self->x[5],
				    self->u[0], self->u[1], self->u[2],
				    self->F[0], self->F[1], self->F[2],
				    self->ParDat->mass);
		
		return &str[0];
	}

}

/* Get rid of warning 401 */
%warnfilter(401) tremolo::Instance;

%rename(TremoloInstance) tremolo::Instance;

%include "md/tremolo/src/tremoloInstance.hh"

%extend tremolo::Instance
{

	/// Get Particle by Id
	/// @param[in]	id	the particle id
	Particle*	GetParticleById(int id)
	{
		return GetParticleById(self->problem_instance.IDs, id);
	}

	/// Get Particle by Id (const version)
	/// @param[in]	id	the particle id
	const Particle* GetParticleById(int id) const
	{
		return GetParticleById(self->problem_instance.IDs, id);
	}

	void CenterOfMass(PyObject *X)
	{
		double x[3];
		self->CenterOfMass(x);
		
		if(PyList_Check(X)) 
                {
			for(int d = 0; d < 3; ++d) 
                        {
				PyList_SetItem(X, d, PyFloat_FromDouble(x[d]));
			}
		} else {
			PyErr_SetString(PyExc_TypeError, "not a list");
			return;
		}
	}

	// Returns the maximal particle Id
	int GetMaxId() const
	{
		int maxId = 0;

		const Particle* p;
		particles_for_each(self,p) {
			maxId = std::max(maxId, (int )p->Id);
		}

		MPI_Allreduce(MPI_IN_PLACE, &maxId, 1, MPI_INT, MPI_MAX, self->comm);
		return maxId;
	}

}

