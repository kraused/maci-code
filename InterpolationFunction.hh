
// vi: tabstop=4:expandtab

#ifndef MACI_INTERPOLATIONFUNCTION_HH_INCLUDED
#define MACI_INTERPOLATIONFUNCTION_HH_INCLUDED 1

#include "Function.hh"
#include "GeoPrimitive.hh"

#include <vector>

// Forwarding
class PtList;

/// A function obtained from interpolating point values. The function manages
/// a list of geometrive primitives together with point values. It evaluates
/// itself at a point by finding a primitive in which the point lies and 
/// interpolating in there. If there is no such point, then the @bkgVal_ is
/// returned.
/// For overlapping primitives, it is the responsibility of the user to ensure
/// that the function value does not depend on the ordering of the primitives
/// For every primitive the user can add a list of values. It is up to the
/// primitive to interpret these values. E.g. for a @Cuboid one value might be
/// interpreted as the value at the center of mass (and therefore a constant
/// interpolation might be used) whereas 4 or 8 are the values at the corners.
class InterpolationFunction : public Function<double,3>
{   
    
public:
    // Create a new instance
    InterpolationFunction(double bkgVal = 0.);
    
    // Copy constructor
    InterpolationFunction(const InterpolationFunction& f);
    
    // Delete the instance
    ~InterpolationFunction();

    // Add a new primitive
    // Returns a reference to @this so that you can concate arguments
    InterpolationFunction& AddNewPrimitive(const GeoPrimitive* P, int N, const double* vals);

    // Returns the number of primitives
    inline int GetNumPrimitives()
    {
        return pr_.size();
    }
    
    // @overrides
    double val(const double* p) const;
    
    // @overrides
    InterpolationFunction* Clone() const;

private:
    // The list of primitives
    std::vector<GeoPrimitive*>  pr_;
    // The list of values
    std::vector<PtList>     pts_;
    // The background value
    double              bkgVal_;
};

#endif

