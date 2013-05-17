
// vi: tabstop=4:expandtab

#ifndef MACI_PYTHON_FUNCTIONPYOBJ_HH_INCLUDED
#define MACI_PYTHON_FUNCTIONPYOBJ_HH_INCLUDED 1

#include "Function.hh"

#include <Python.h>

/// Implementation which uses a python function ptr (object)
template<typename Tp, int N>
class FunctionPyObj : public Function<Tp, N>
{

public:
    /// Construct a new instance
    FunctionPyObj(PyObject* func)
    : F(func)
    {
        Py_INCREF(F);
    }
    
    /// Copy constructor
    FunctionPyObj(const FunctionPyObj<Tp,N>& other)
    : F(other.F)
    {
        Py_INCREF(F);
    }
    
    /// @overrides
    Tp val(const double* p) const
    {
        double    v;
        PyObject* obj;
        PyObject* item;

        if(1 == N)
        {
            char str[] = "f";
            obj = PyEval_CallFunction(F, str, p[0], p[1], p[2]);
        }
        if(2 == N)
        {
            char str[] = "f,f";
            obj = PyEval_CallFunction(F, str, p[0], p[1], p[2]);
        }
        if(3 == N)
        {
            char str[] = "f,f,f";
            obj = PyEval_CallFunction(F, str, p[0], p[1], p[2]);
        }
        if(4 == N)
        {
            char str[] = "f,f,f,f";
            obj = PyEval_CallFunction(F, str, p[0], p[1], p[2]);
        }   

        if(!obj) 
        {
            PyErr_Print();
            abort();
        }
        if(!PyNumber_Check(obj)) 
        {
            PyErr_SetString(PyExc_TypeError,"not a number");
            PyErr_Print();
            abort();
        }

        item = PyNumber_Float(obj);
        v    = PyFloat_AsDouble(item);

        Py_DECREF(obj);
        Py_DECREF(item);

        return v;
    }
    
    /// Destroy the instance
    ~FunctionPyObj()
    {
        Py_DECREF(F);
    }
    
    /// @overrides
    FunctionPyObj* Clone() const
    {
        return new FunctionPyObj<Tp,N>(*this);
    }
    
private:
    /// The function pointer
    PyObject* F;
};

#endif

