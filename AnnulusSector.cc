
// vi: tabstop=4:expandtab

#include "AnnulusSector.hh"
#include "exception/BadArgException.hh"

AnnulusSector::AnnulusSector(const float* x, const float* r, const float* theta)
{
    for(int d = 0; d < 2; ++d) {
        x_[d] = x[d];
        r_[d] = r[d];
        theta_[d] = theta[d];
    }
}

AnnulusSector::AnnulusSector(const AnnulusSector& S)
{
    for(int d = 0; d < 2; ++d) {
        x_[d] = S.x_[d];
        r_[d] = S.r_[d];
        theta_[d] = S.theta_[d];
    }   
}

bool AnnulusSector::Intersects(const Box& b) const
{
    abort();
    return false;
}

bool AnnulusSector::IsInside(const double* p) const
{
    float s, phi;

    /* Compute polar coordinates (s, phi) */
    s = phi = 0.;
    
    if(  s <     r_[0] + 1e-7 ||   s >     r_[1] - 1e-7 ||
       phi < theta_[0] + 1e-7 || phi > theta_[1] - 1e-7) {
        return false;
    }
    
    return true;
}

AnnulusSector* AnnulusSector::Clone() const
{
    return new AnnulusSector(*this);
}

double AnnulusSector::Interpolate(int npts,
            const double* vals, const double* p) const
{
    if(1 == npts) {
        return vals[0];
    }
    
    throw exception::BadArgException();
}

