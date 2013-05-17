
// vi: tabstop=4:expandtab

#ifndef MACI_VECTOR_FUNCTION_HH_INCLUDED
#define MACI_VECTOR_FUNCTION_HH_INCLUDED 1

/// Interface for all vector valued functions
template<typename Tp, int N, int M>
class VectorFunction
{
    
public:
    /// Evaluate the function
    /// @param[in]  p   the point to evaluate at. p should have
    ///         exactly N components
    /// @param[out] v   the value. This has exactly M components
    virtual void val(const double* p, Tp* v) const = 0;

    /// Clone the instance
    virtual VectorFunction* Clone() const = 0;

    /// Shut up ...
    virtual ~VectorFunction() { }
};

#endif

