
// vi: tabstop=4:expandtab

#ifndef MACI_INTERPOLATION_WEIGHTS_HH_INCLUDED
#define MACI_INTERPOLATION_WEIGHTS_HH_INCLUDED 1

#include "InterpolationFunction.hh"
#include "Weights.hh"

// A weighting function obtained from interpolating between values
class InterpolationWeights : public InterpolationFunction, public Weights
{

public:
    // Constructor
    // @warn Since the geometric primitives are added later on we cannot
    //       call @Update in the constructor
    InterpolationWeights(double bkgVal = 0.)
    : InterpolationFunction(bkgVal)
    {
    }
    
    // @overrides
    double val(const double* p) const
    {
        return InterpolationFunction::val(p);
    }
};


#endif

