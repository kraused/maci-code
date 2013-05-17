
// vi: tabstop=4:expandtab

#ifndef MACI_QUAD_GAUSS_POINT_HH_INCLUDED
#define MACI_QUAD_GAUSS_POINT_HH_INCLUDED 1

#include "quad/GaussPointPayload.hh"

namespace quad
{

    /// GaussPoint: A 
    template<typename Tp, int D>
    class GaussPoint
    {

    public:
        /// Create a new GaussPoint instance
        GaussPoint(const GaussPointPayload<Tp>* Q)
        : P(Q)
        {   }

        /// Get local coordinates
        const Tp* local() const
        {
            return P->lpt;
        }

        /// Get global coordinates
        const Tp* global() const
        {
            return P->gpt;
        }

        /// Get the weight
        Tp weight() const
        {
            return P->wgt;
        }

        /// Jump to the next Gauss point
        GaussPoint& operator++()
        {
            ++P;
            return (*this);
        }

        /// Compare to points
        /// @param other    the other point     
        /// @returns        true if the points do not match
        bool operator!=(const GaussPoint<Tp, D>& other)
        {
            return other.P != P;
        }

    private:
        /// Pointer to an internal structure
        /// holding the actual data. This simplifies
        /// the implmentation of the increment
        /// operator
        const GaussPointPayload<Tp>* P;
    
    };

}

#endif

