
// vi: tabstop=4:expandtab

#ifndef MACI_GEOPRIMITIVES_HH_INCLUDED
#define MACI_GEOPRIMITIVES_HH_INCLUDED 1

#include "polylib/polylib.hh"

/// Geometric primitive which can be used to build
/// more complex geometric constructions by union
/// and intersection
///
/// GeoPrimitives are supposed to implement a copy
/// constructor!
class GeoPrimitive
{
/* protected: */
/* for SWIG */public:
    typedef polylib::Box<double,DIM>    Box;

public:
    // Returns @true if the box @b intersects
    // the primitive. The method is allowed
    // to return @true also for boxes which do
    // not intersect.
    virtual bool Intersects(const Box& b) const = 0;
    
    // Returns true if a point @p is in the
    // primitive
    virtual bool IsInside(const double* p) const = 0;
    
    // Returns value of interpolated function at point @p. The interpretation
    // of the input values is left to the implementing class. For example,
    // if @npts is 1, the value might be the value at the midpoint whereas
    // higher number of points might correspond to values at corners.
    // Obviously, the order of the interpolation method applied depends on
    // the number of input points.
    // A given implementation might not support abitrary values of @npts and
    // might throw an exception of type @BadArgException if a wrong number 
    // of input values is passed in.
    // @param npts  the number of points with known values
    // @param vals  the values of the function at the above points
    virtual double Interpolate(int npts, const double* vals, const double* p) const = 0;
    
    // Make the compiler lucky ...
    virtual ~GeoPrimitive() { } 
    
    // Default constructor
    GeoPrimitive()
    { }
    
    // Copy constructor is supposed to be always
    // implemented by subclasses
    GeoPrimitive(const GeoPrimitive& other)
    { }
    
    // Clone function
    // virtual constructor idiom
    virtual GeoPrimitive* Clone() const = 0;
};

#endif

