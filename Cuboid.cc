
// vi: tabstop=4:expandtab

#include "Cuboid.hh"

static double Q1ShapeFunction(int c, const double* p)
{
#if 2 == DIM
    switch(c) 
    {
        case 0: return ((1.0-p[0])*(1.0-p[1]));
        case 1: return (p[0]*(1.0-p[1]));
        case 2: return (p[0]*p[1]);
        case 3: return ((1.0-p[0])*p[1]);
    }
#endif
#if 3 == DIM
    switch(c) 
    {
        case 0: return ((1.0-p[0])*(1.0-p[1])*(1.0-p[2]));
        case 1: return ((p[0])*(1.0-p[1])*(1.0-p[2]));
        case 2: return ((p[0])*(p[1])*(1.0-p[2]));
        case 3: return ((1.0-p[0])*(p[1])*(1.0-p[2]));
        case 4: return ((1.0-p[0])*(1.0-p[1])*(p[2]));
        case 5: return ((p[0])*(1.0-p[1])*(p[2]));
        case 6: return ((p[0])*(p[1])*(p[2]));
        case 7: return ((1.0-p[0])*(p[1])*(p[2]));
    }
#endif    

    CHECK_DEBUG(1 == 0, "Should not come here.");
    return 0.;
}

// Function to interpolate values at the corners of a box 
double InterpolateBoxCornerValues(const polylib::Box<double,DIM>& box, const double* vals, const double* p)
{
    int    i, c;
    double q[3], v;

    // Local coordinates
    for(i = 0; i < DIM; ++i)
        q[i] = (p[i] - box.getLower()[i])/box.getSidelength(i);
    
    v = 0.0;
    for(c = 0; c < box.ncorners(); ++c)
        v += Q1ShapeFunction(c, q)*vals[c];
    
    return v;
}

