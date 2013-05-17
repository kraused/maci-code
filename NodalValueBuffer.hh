
// vi: tabstop=4:expandtab

#ifndef MACI_NODAL_VALUE_BUFFER_HH_INCLUDED
#define MACI_NODAL_VALUE_BUFFER_HH_INCLUDED 1

#include <valarray>
#include <stdio.h>

#include "DesignByContract.hh"
#include "NodeList.hh"
#include "MeshMassMatrix.hh"

#include "fe/FiniteElements.hh"


/// Buffer for nodal values 
/// 
/// NodalValueBuffer allows to buffer the values of a mesh nodal quantity in a
/// contiguous array
template<typename Float>
class NodalValueBuffer : public std::valarray<Float>
{
    typedef std::valarray<Float> Base;

protected:
    typedef fe::interface::Mesh Mesh;
    typedef fe::interface::Node Node;

public:
    /// Create a new instance
    /// @param nodes    the 
    NodalValueBuffer(const NodeList& nodes)
    : Base(3*nodes.size()), list(nodes)
    {
    }

    /// Delete the buffer
    ~NodalValueBuffer()
    {
    }

    /// Execute function on all data items
    /// @param f    the functor must implement the function
    ///     <code>
    ///     void operator()(Node nd, Float *x)
    ///     </code>
    ///     or
    ///     <code>
    ///     void operator()(Node nd, const Float *x)
    ///     </code>
    ///     It is allowed to read and write data as needed
    template<typename Functor>
    void forall(const Functor& f)
    {
        std::size_t i = 0;
        for(NodeList::Iterator nd = list.begin(); nd != list.end(); ++nd, ++i) 
        {
            CHECK_DEBUG(i < this->size(), "buffer overflow");
            f(*nd, &(*this)[3*i]);
        }

        CHECK_ALWAYS(3*i <= this->size(), "buffer overflow");
    }

    /// BLAS Level 1 axpy operation
    /// @param alpha    factor
    /// @param x        the first vector
    /// @param y        the updated vector
    static void axpy(Float alpha, const NodalValueBuffer<Float>& x, NodalValueBuffer<Float>& y)
    {
        CHECK_PRECOND_DEBUG(y.size() == x.size());
        
        for(size_t i = 0; i < x.size(); ++i)
            y[i] += alpha*x[i];
    }

    /// Get the l2 norm of the values stored
    /// @returns norm >= 0.
    Float norml2() const
    {
        Float n = 0.;
        for(std::size_t i = 0; i < this->size(); ++i) {
            n += (*this)[i]*(*this)[i];
        }
        return n;
    }

    /// Assign value to every element
    NodalValueBuffer& operator=(Float val)
    {
        Base::operator=(val);
        return (*this);
    }

private:
    /// The nodal list
    NodeList list;
};

#endif

