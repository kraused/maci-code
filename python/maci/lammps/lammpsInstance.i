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
#include "md/lammps/src/lammpsInstance.hh"
%}

%typemap(in) (const double midpt[])
{
	if(PyList_Check($input)) {
		$1 = new double[3];

		$1[0] = PyInt_AsLong(PyNumber_Int(PyList_GetItem($input,0)));
		$1[1] = PyInt_AsLong(PyNumber_Int(PyList_GetItem($input,1)));
		$1[2] = PyInt_AsLong(PyNumber_Int(PyList_GetItem($input,2)));
	} else {
		PyErr_SetString(PyExc_TypeError, "not a list");
		return 0;
	}
}

%typemap(freearg) (const double midpt[])
{
	delete[] $1;
}

/* Get rid of warning 401 */
%warnfilter(401) lammps::Instance;

%rename(LammpsInstance) lammps::Instance;

%include "md/lammps/src/lammpsInstance.hh"

%extend lammps::Instance
{

	/// Wrapper around the centerOfMass 
	void CenterOfMass(PyObject *X)
	{
		double Y[3];
		self->centerOfMass(Y);
		
		if(PyList_Check(X)) {
			for(int d = 0; d < 3; ++d) {
				PyList_SetItem(X, d, PyFloat_FromDouble(Y[d]));
			}
		} else {
			PyErr_SetString(PyExc_TypeError, "not a list");
			return;
		}	
	}

        /// Get the position of a particle from its local id
        double ParticlePos(int i, int j)
        {
                return self->x(i)[j];
        }

        /// Set the displacement of a particle
        void SetParticleDispl(int i, double ux, double uy, double uz)
        {
                self->x(i)[0] += ux;
                self->x(i)[1] += uy;
                self->x(i)[2] += uz;

                // TODO Do we need to communicate particles here?
                //      If the discplacement is too large we are
                //      screwed anyway
        }

        /// Set the velocity of a particle
        void SetParticleVel(int i, double vx, double vy, double vz)
        {
                self->v(i)[0] = vx;
                self->v(i)[1] = vy;
                self->v(i)[2] = vz;
        }

};

