
// vi: tabstop=4:expandtab

#ifndef MACI_CUBOID_HH_INCLUDED
#define MACI_CUBOID_HH_INCLUDED

#include "GeoPrimitive.hh"
#include "ProcElement.hh"
#include "exception/BadArgException.hh"

#include "polylib/polylib.hh"

/// A Box as a geometric primitive
class Cuboid : public GeoPrimitive
{
/* for SWIG */public:
    typedef GeoPrimitive    Base;

public:
    // Construct a new instance of a sector
    Cuboid(const polylib::Box<double,DIM>& b)
    : box_(b), vol_(b.getVolume())
    {
    }
    
    // Copy constructor
    Cuboid(const Cuboid& C);
    
    // @overrides
    bool Intersects(const Base::Box& b) const;
    // @overrides
    bool IsInside(const double* p) const;
    
    // @overides
    virtual Cuboid* Clone() const; 
    
    // @overrides
    virtual double Interpolate(int npts, const double* vals, const double* p) const; 
    
private:
    /// The box
    const Base::Box box_;
    /// The volume of the box
    const double    vol_;
    /// Tolerance value
    double          M_tol;
};


Cuboid::Cuboid(const Cuboid& C)
: box_(C.box_), vol_(C.vol_), M_tol(gcpe()->absTolerance()*gcpe()->charLength())
{
}

bool Cuboid::Intersects(const Base::Box& b) const
{
    // We assume typically that the mesh resolves the interface between the
    // handshake region and the pure FE region so that a sufficiently large
    // tolerance should be used to ensure that no wrong nodes are taken
    // into account
    return box_.hasIntersection(b, 0.9*b.getVolume()/*0.05*gcpe()->charLength()*/);
}

bool Cuboid::IsInside(const double* p) const
{
    return box_.isInside(p, M_tol);
}

Cuboid* Cuboid::Clone() const
{
    return new Cuboid(*this);
}

// Function to interpolate values at the corners of a box 
double InterpolateBoxCornerValues(const polylib::Box<double,DIM>& box, const double* vals, const double* p);

double Cuboid::Interpolate(int npts, const double* vals, const double* p) const
{
    if(1 == npts)
        return vals[0];
    
    if(box_.ncorners() == npts) {
        return InterpolateBoxCornerValues(box_, vals, p);
    }
    
    throw exception::BadArgException();
}

#endif


