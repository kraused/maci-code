
// vi: tabstop=4:expandtab

#ifndef POLYLIB_BOX_INTERSECTION_HH_INCLUDED
#define POLYLIB_BOX_INTERSECTION_HH_INCLUDED 1

#include <algorithm>
#include <assert.h>
#include <stdio.h>

namespace polylib 
{

    /// Check if two boxes have an intersection
    /// @template_param Tp      the polytope type.
    /// @template_param Float   the floating point type
    /// @param x    the first box
    /// @param y    the second box
    /// @param tol  the tolerance to use
    template<typename Tp, typename Float>
    bool boxesHaveIntersection(const Tp& x, const Tp& y, Float tol)
    {
        return !( (y.getLower()[0] > x.getUpper()[0] + tol) ||
                  (y.getLower()[1] > x.getUpper()[1] + tol) ||
                  (y.getLower()[2] > x.getUpper()[2] + tol) ||
                  (y.getUpper()[0] < x.getLower()[0] - tol) ||
                  (y.getUpper()[1] < x.getLower()[1] - tol) ||
                  (y.getUpper()[2] < x.getLower()[2] - tol) );
    }

    /// Helper function which computes the lower and upper corner of the
    /// intersection polytope if a corner of the polytope y is inside the
    /// polytope x
    /// @returns 1 on success and 0 otherwise
    template<typename Tp, typename Float>
    static int intersectBoxesInclosedCorners(const Tp& x, const Tp& y, Float *lower, Float *upper)
    {
        ASSERT(x.ncorners() == y.ncorners());

        for(int i = 0; i < y.ncorners(); ++i)
        {
            if(not x.isInside(y.getCorner(i), 1e-7))
                continue;

            switch(i) 
            {
            case 0:
                std::copy(y.getLower(), y.getLower()+3, lower);

                upper[0] = std::min(y.getUpper()[0],x.getUpper()[0]);
                upper[1] = std::min(y.getUpper()[1],x.getUpper()[1]);
                upper[2] = std::min(y.getUpper()[2],x.getUpper()[2]);
                break;
            case 1:
                lower[0] = std::max(y.getLower()[0],x.getLower()[0]);
                lower[1] = y.getLower()[1];
                lower[2] = y.getLower()[2];

                upper[0] = y.getUpper()[0];
                upper[1] = std::min(y.getUpper()[1],x.getUpper()[1]);
                upper[2] = std::min(y.getUpper()[2],x.getUpper()[2]);
                break;
            case 2:
                lower[0] = std::max(y.getLower()[0],x.getLower()[0]);
                lower[1] = std::max(y.getLower()[1],x.getLower()[1]);
                lower[2] = y.getLower()[2];

                upper[0] = y.getUpper()[0];
                upper[1] = y.getUpper()[1];
                upper[2] = std::min(y.getUpper()[2],x.getUpper()[2]);
                break;
            case 3:
                lower[0] = std::max(y.getLower()[0],x.getLower()[0]);
                lower[1] = std::max(y.getLower()[1],x.getLower()[1]);
                lower[2] = y.getLower()[2];

                upper[0] = x.getUpper()[0];
                upper[1] = y.getUpper()[1];
                upper[2] = std::min(y.getUpper()[2],x.getUpper()[2]);
                break;
            case 4:
                lower[0] = std::max(y.getLower()[0],x.getLower()[0]);
                lower[1] = std::max(y.getLower()[1],x.getLower()[1]);
                lower[2] = std::max(y.getLower()[2],x.getLower()[2]);

                upper[0] = std::min(y.getUpper()[0],x.getUpper()[0]);
                upper[1] = std::min(y.getUpper()[1],x.getUpper()[1]);
                upper[2] = y.getUpper()[2];
                break;
            case 5:
                lower[0] = std::max(y.getLower()[0],x.getLower()[0]);
                lower[1] = y.getLower()[1];
                lower[2] = std::max(y.getLower()[2],x.getLower()[2]);

                upper[0] = y.getUpper()[0];
                upper[1] = std::min(y.getUpper()[1],x.getUpper()[1]);  
                upper[2] = y.getUpper()[2];
                break;
            case 6:
                lower[0] = std::max(y.getLower()[0],x.getLower()[0]);
                lower[1] = std::max(y.getLower()[1],x.getLower()[1]);
                lower[2] = std::max(y.getLower()[2],x.getLower()[2]);

                std::copy(y.getUpper(), y.getUpper()+3, upper);
                break;
            case 7:
                lower[0] = y.getLower()[0];
                lower[1] = std::max(y.getLower()[1],x.getLower()[1]);
                lower[2] = x.getLower()[2];

                upper[0] = std::min(y.getUpper()[0],x.getUpper()[0]);
                upper[1] = std::min(y.getUpper()[1],x.getUpper()[1]);
                upper[2] = y.getUpper()[2];
                break;                
            default:
                abort();
            }

            return 1;
        }

        return 0;
    }

    /// Helper function which computes the lower and upper corner of the
    /// intersection polytope in the slim cases where no corner of the
    /// the polytopes lie in the other polytope
    /// @returns 1 on success and 0 otherwise
    template<typename Tp, typename Float>
    static int intersectBoxesSlimCases(const Tp& x, const Tp& y, Float *lower, Float *upper)
    {
        int d, k;

        for(d = 0; d < 3; ++d) 
            if(y.getLower()[d] >= x.getLower()[d] and y.getUpper()[d]  < x.getUpper()[d]) 
            {
                for(k = 0; k < 3; ++k) 
                {
                    lower[k] = (k == d) ? y.getLower()[k] : std::max(x.getLower()[k],y.getLower()[k]);
                    upper[k] = (k == d) ? y.getUpper()[k] : std::min(x.getUpper()[k],y.getUpper()[k]);
                }

                return 1;
            }

        return 0;
    }

    /// Compute the intersection of two polytope
    /// @template_param Tp  the box type. Must allow for access to the
    ///         corners using the function
    ///         <code>
    ///         Float getCorner(int i, int d)
    ///         </code>
    /// @param x    the first polytope
    /// @param y    the second polytope
    /// @param tol  the tolerance for the intersection check
    template<typename Tp, typename Float>
    Tp intersectBoxes(const Tp& x, const Tp& y, Float tol)
    {
        if(x.getVolume() < tol || y.getVolume() < tol) 
            return Tp();
        if(not boxesHaveIntersection(x,y,tol))
            return Tp();

        Float lower[3], upper[3];
        std::fill(lower, lower+3, 0.0);
        std::fill(upper, upper+3, 0.0);

        if(intersectBoxesInclosedCorners(x,y,lower,upper))
            goto END;
        if(intersectBoxesInclosedCorners(y,x,lower,upper))
            goto END;

        if(intersectBoxesSlimCases(x,y,lower,upper))
            goto END;
        if(intersectBoxesSlimCases(y,x,lower,upper))
            goto END;
    
    END:
        if(lower[0] > upper[0]+1e-7 || lower[1] > upper[1]+1e-7 || lower[2] > upper[2]+1e-7)
        {
            printf(" WARNING at %s:%d: Don't trust the cut polytope: "
                   "( [ %e, %e ] x [ %e, %e ] x [ %e, %e ] ) c ( [ %e, %e ] x [ %e, %e ] x [ %e, %e ] ) ?= "
                   "( [ %e, %e ] x [ %e, %e ] x [ %e, %e ] )\n", __FILE__, __LINE__,
                   x.getLower()[0], x.getUpper()[0], x.getLower()[1], x.getUpper()[1], x.getLower()[2], x.getUpper()[2],
                   y.getLower()[0], y.getUpper()[0], y.getLower()[1], y.getUpper()[1], y.getLower()[2], y.getUpper()[2],
                   lower[0], upper[0], lower[1], upper[1], lower[2], upper[2]);

            /// Make sure the cut polytope has volume zero.
            upper[0] = lower[0];
            upper[1] = lower[1];
            upper[2] = lower[2];
        }

        return Tp(lower, upper);
    }

}

#endif

