
// vi: tabstop=4:expandtab

#include "python/maci/WeightsPyObj.hh"
#include "ProcElement.hh"

WeightsPyObj::WeightsPyObj(PyObject* func)
: FunctionPyObj<double,3>(func)
{
    Weights::Update(*gcpe());
}

