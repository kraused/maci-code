
// vi: tabstop=4:expandtab

#ifndef POLYLIB_BOX_HH_INCLUDED
#define POLYLIB_BOX_HH_INCLUDED 1

#include "polylib/BoxIntersection.hh"
#include "polylib/BoxRefine.hh"

namespace polylib {

    /// A axis parallel cuboid
    ///
    /// @warning Use this class only for <code>D == 2</code> or <code>D == 3</code>
    template<typename Float, int D>
    class Box
    {
    private:
        static const int sign2[][2];
        static const int sign3[][3];

        /// Compute corners from lower and upper corners
        template<typename Float2>
        void boundsToCorners(const Float2 *lower, const Float2 *upper)
        {
            std::fill(corner, corner + 3*8, 0.0);

            if(2 == D) 
            {
                for(int i = 0; i < (1<<D); ++i)
                    for(int k = 0; k < D; ++k)
                        getCorner(i)[k] = 0.5*((1.-sign2[i][k])*lower[k] + (1.+sign2[i][k])*upper[k]);
            }
            if(3 == D) 
            {
                for(int i = 0; i < (1<<D); ++i)
                    for(int k = 0; k < D; ++k)
                        getCorner(i)[k] = 0.5*((1.-sign3[i][k])*lower[k] + (1.+sign3[i][k])*upper[k]);
            }
        }

    public:
        /// Construct an empty box
        Box()
        {
            std::fill(corner, corner + 3*8, 0);
        }
        
        /// Construct a box from a lower and an upper corner
        template<typename Float2>
        Box(const Float2 *lower, const Float2 *upper)
        {
            boundsToCorners(lower,upper);
        }

        /// Copy constructor
        /// @param o    the other instance
        Box(const Box<Float,D> &o)
        {
            std::copy(o.corner, o.corner + 3*8, corner);
        }

        /// Assignment operator
        /// @param o    the other instance
        const Box<Float,D> &operator=(const Box<Float,D> &o)
        {
            std::copy(o.corner, o.corner + 3*8, corner);
            return (*this);
        }

        /// Get a pointer to a corner
        /// @param i    the corner index
        inline Float* getCorner(int i)
        {
            ASSERT(i >= 0 and i < ncorners());
            return corner + (i*3);
        }

        /// Get a pointer to a corner (const version)
        /// @param i    the corner index
        inline const Float* getCorner(int i) const
        {
            ASSERT(i >= 0 and i < ncorners());
            return corner + (i*3);
        }

        /// Get a pointer to the lowest corner
        inline Float* getLower()
        {
            return getCorner(0);
        }

        /// Get a pointer to the lowest corner (const version)
        inline const Float* getLower() const
        {
            return getCorner(0);
        }
    
        /// Get a pointer to the upper corner
        inline Float* getUpper()
        {
            return getCorner(D*(D-1));
        }

        /// Get a pointer to the upper corner (const version)
        inline const Float* getUpper() const
        {
            return getCorner(D*(D-1));
        }

        /// Check if a point is in the box
        template<typename Float2>
        bool isInside(const Float2 *p, Float tol) const
        {
            for(int k = 0; k < D; ++k) 
            {
                if(p[k] < getLower()[k]-tol || p[k] > getUpper()[k]+tol)
                    return false;
            }

            return true;
        }

        /// Get the number of corners
        int ncorners() const
        {
            return (1 << D);
        }

        /// Get sidelength of the polytope
        /// @param d    the direction
        inline Float getSidelength(int d) const
        {
            return (getUpper()[d] - getLower()[d]);
        }

        /// Get the midpoint of the box
        /// @param d    the dimension
        inline Float getMidpoint(int d) const
        {
            return 0.5*(getLower()[d] + getUpper()[d]);
        }

        /// Get the volume of the polytope
        inline Float getVolume() const
        {
            Float vol = 1.;
            for(int k = 0; k < D; ++k)
                vol *= getSidelength(k);

            return vol;
        }

        /// Check if two boxes have a nontrivial intersection
        /// This is the fast but unsafe method, i.e. the cut
        /// could be very small or even lower dimensional
        /// @param o    the other box
        bool hasIntersectionUnsafe(const Box<Float,D>& o) const
        {
            return boxesHaveIntersection(*this, o, 1e-6);
        }
    
        // Check if two boxes have a nontrivial intersection
        // A tolerance (which is the volume of the resulting
        // cut polytope) must be passed
        /// @param o    the other box
        bool hasIntersection(const Box<Float,D>& o, Float tol) const
        {
            return (intersectBoxes(*this, o, 1e-6).getVolume() >= tol);
        }
    
        /// Compute the intersection of two boxes. The box has volume
        /// 0.0 if the polytopes do not intersect
        Box<Float,D> getIntersectionUnsafe(const Box<Float, D>& o) const
        {
            return intersectBoxes(*this, o, 1e-6);
        }
    
        /// Compute the intersection of two boxes. The box has volume
        /// 0.0 if the polytope cut volume is below the treshold
        Box<Float,D> getIntersection(const Box<Float,D>& o, Float tol) const
        {
            Box<Float,D> cut = intersectBoxes(*this, o, tol);
            if(cut.getVolume() < tol) 
                return Box<Float,D>();
            else
                return cut;
        }

        /// Refine the box uniform and give the output in the subboxes
        void refine(Box<Float,D> *subboxes) const
        {
            if(2 == D) {
                refineBox2<Box<Float,D>,Float>(*this,subboxes);
            }
            if(3 == D) {
                refineBox3<Box<Float,D>,Float>(*this,subboxes);
            }
        }
    
        /// Reset the corners
        /// @param lower the lower corner coordinates
        /// @param upper the upper corner coordinates
        template<typename Float2>
        void reset(const Float2 *lower, const Float2 *upper)
        {
            boundsToCorners(lower,upper);
        }

    private:
        /// The corners of the polytope
        Float corner[8*3];
    };

    template<typename Float, int D>
    const int Box<Float,D>::sign2[][2] = { {-1, -1}, {+1, -1}, 
                           {+1, +1}, {-1, +1} };

    template<typename Float, int D>
    const int Box<Float,D>::sign3[][3] = { {-1, -1, -1}, {+1, -1, -1}, 
                           {+1, +1, -1}, {-1, +1, -1}, 
                           {-1, -1, +1}, {+1, -1, +1},
                           {+1, +1, +1}, {-1, +1, +1} };

}

#endif

