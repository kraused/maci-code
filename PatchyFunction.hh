
// vi: tabstop=4:expandtab

#ifndef MACI_PATCHYFUNCTION_HH_INCLUDED
#define MACI_PATCHYFUNCTION_HH_INCLUDED

#include "Function.hh"
#include "GeoPrimitive.hh"

#include <vector>

/// PatchyFunction: A function constructed by mergeing functions defined on 
///         disjoint patches
class PatchyFunction : public Function<double,3>
{

public:
    // Create a new instance
    PatchyFunction(double bkgVal = 0.);
    
    // Copy constructor
    PatchyFunction(const PatchyFunction& f);
    
    // Delete the instance
    ~PatchyFunction();

    // Add a new primitive
    // Returns a reference to @this so that you can concate arguments
    PatchyFunction& AddNewPrimitive(const GeoPrimitive* P, const Function<double,3>* f);

    // Returns the number of primitives
    inline int GetNumPrimitives()
    {
        return pr_.size();
    }
    
    // @overrides
    double val(const double* p) const;

    // @overrides
    PatchyFunction* Clone() const;

private:
    // The list of primitives
    std::vector<GeoPrimitive*>   pr_;
    // The list of values
    std::vector<Function<double,3>*> fct_;
    // The background value
    double               bkgVal_;

};

#endif

