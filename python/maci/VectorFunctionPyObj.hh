
// vi: tabstop=4:expandtab

#ifndef MACI_PYTHON_VECTOR_FUNCTIONPYOBJ_HH_INCLUDED
#define MACI_PYTHON_VECTOR_FUNCTIONPYOBJ_HH_INCLUDED 1

#include "VectorFunction.hh"

#include <Python.h>

/// Implementation which uses a python function ptr (object)
template<typename Tp, int N, int M>
class VectorFunctionPyObj : public VectorFunction<Tp, N, M>
{

public:
    /// Construct a new instance
    VectorFunctionPyObj(PyObject* func)
    : F(func)
    {
        Py_INCREF(F);
    }
    
    /// Copy constructor
    VectorFunctionPyObj(const VectorFunctionPyObj<Tp,N,M>& other)
    : F(other.F)
    {
        Py_INCREF(F);
    }
    
    /// @overrides
    void val(const double* p, Tp* v) const
    {
        PyObject* obj;
        PyObject* member;
        PyObject* item;
        int       i;

        if(1 == N)
        {
            char str[] = "f";
            obj = PyEval_CallFunction(F, str, p[0]);
        }
        if(2 == N)
        {
            char str[] = "f,f";
            obj = PyEval_CallFunction(F, str, p[0], p[1]);
        }
        if(3 == N)
        {
            char str[] = "f,f,f";
            obj = PyEval_CallFunction(F, str, p[0], p[1], p[2]);
        }
        if(4 == N)
        {
            char str[] = "f,f,f,f";
            obj = PyEval_CallFunction(F, str, p[0], p[1], p[2], p[3]);
        }   

        if(!obj) 
        {
            PyErr_Print();
            abort();
        }
        if(!PyList_Check(obj))
        {
            PyErr_SetString(PyExc_TypeError,"not a list");
            PyErr_Print();
            abort();
        }
        if(M != PyList_Size(obj))
        {
            PyErr_SetString(PyExc_AssertionError,"size of list is wrong");
            PyErr_Print();
            abort();
        }

        for(i = 0; i < M; ++i)
        {
            member = PyList_GetItem(obj, i);

            if(!PyNumber_Check(member))
            {
                PyErr_SetString(PyExc_TypeError,"not a number");
                PyErr_Print();
                abort();
            }

            item = PyNumber_Float(member);
            v[i] = PyFloat_AsDouble(item);
            
            Py_DECREF(item);
        }
        
        Py_DECREF(obj);
    }
    
    /// Destroy the instance
    ~VectorFunctionPyObj()
    {
        Py_DECREF(F);
    }
    
    /// @overrides
    VectorFunctionPyObj* Clone() const
    {
        return new VectorFunctionPyObj<Tp,N,M>(*this);
    }
    
private:
    /// The function pointer
    PyObject* F;
};

#endif

