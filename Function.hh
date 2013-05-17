
// vi: tabstop=4:expandtab

#ifndef MACI_FUNCTION_HH_INCLUDED
#define MACI_FUNCTION_HH_INCLUDED 1

/// Interface for all functions
template<typename Tp, int N>
class Function
{
    
public:
    /// Evaluate the function
    /// @param[in]  p   the point to evaluate at. p should have
    ///         exactly N components
    virtual Tp val(const double* p) const = 0;

    /// Clone the instance
    virtual Function* Clone() const = 0;

    /// Shut up ...
    virtual ~Function() { }
};

#endif

