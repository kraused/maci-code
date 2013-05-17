
// vi: tabstop=4:expandtab

#include "Element.hh"

using namespace fe::interface;

void Element::BoundingBox(double* lower, double* upper) const
{
    int i, k;

    for(i = 0; i < DIM; ++i)
    {
        lower[i] = corner(0).coords()[i];
        upper[i] = corner(0).coords()[i];
    }

    for(k = 0; k < ncorners(); ++k)
        for(i = 0; i < DIM; ++i)
        {
            lower[i] = std::min(lower[i], corner(k).coords()[i]);
            upper[i] = std::max(upper[i], corner(k).coords()[i]);
        }

    if(2 == DIM)
        lower[2] = upper[2] = 0.0;
}

