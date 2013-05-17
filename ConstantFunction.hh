
// vi: tabstop=4:expandtab

#ifndef MACI_CONSTANTFUNCTION_HH_INCLUDED
#define MACI_CONSTANTFUNCTION_HH_INCLUDED 1

#include "Function.hh"

/// A constant function
template<typename Tp, int N>
class ConstantFunction : public Function<Tp,N>
{

public:
    /// @overrides
    double val(const double* p) const
    {
        return v;
    }
    
    /// Construct a new instance
    /// @param[in]  w   the value
    ConstantFunction(double w = 1.)
    : v(w)
    {
    }
    
    // Copy constructor
    ConstantFunction(const ConstantFunction<Tp,N>& other)
    : v(other.v)
    {
    }
    
    /// @overrides
    Function<Tp,N>* Clone() const
    {
        return new ConstantFunction<Tp,N>(*this);
    }
    
private:
    /// the value
    double  v;

};

#endif

