
// vi: tabstop=4:expandtab

#ifndef MACI_PYTHON_PYOBJWEIGHTS_HH_INCLUDED
#define MACI_PYTHON_PYOBJWEIGHTS_HH_INCLUDED 1

#include "python/maci/FunctionPyObj.hh"
#include "Weights.hh"
#include "exception/BadCallException.hh"

#include <Python.h>

/// Implementation which uses a python function ptr (object)
class WeightsPyObj : public FunctionPyObj<double,3>, public Weights
{

public:
    /// @overrides
    double val(const double* p) const
    {
        return FunctionPyObj<double,3>::val(p);
    }

    /// @overrides
    FunctionPyObj<double,3>* Clone() const
    {
        throw exception::BadCallException();
        return 0;
    }

    /// Construct a new instance
    WeightsPyObj(PyObject* func);

};

#endif

