
// vi: tabstop=4:expandtab

#ifndef MACI_ANNULUSSECTOR_HH_INCLUDED
#define MACI_ANNULUSSECTOR_HH_INCLUDED 1

#include "GeoPrimitive.hh"

// A sector of an angulus
class AnnulusSector : public GeoPrimitive
{
/* for SWIG */public:
    typedef GeoPrimitive    Base;
    typedef float       float3[3];
    typedef float       float2[2];

public:
    // Construct a new instance of a sector
    AnnulusSector(const float* x, const float* r, const float* theta);
    
    // Copy constructor
    AnnulusSector(const AnnulusSector& S);
    
    // @overrides
    bool Intersects(const Base::Box& b) const;
    // @overrides
    bool IsInside(const double* p) const;
    
    // @overides
    virtual AnnulusSector* Clone() const; 
    
    // @overrides
    virtual double Interpolate(int npts, 
            const double* vals, const double* p) const;
        
    
private:
    // Center point
    float3  x_;
    // inner and outer radius
    float2  r_;
    // angle range
    float2  theta_;
};

#endif

