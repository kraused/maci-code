
// vi: tabstop=4:expandtab

#include "Mesh.hh"

using namespace fe::interface;

void Mesh::BoundingBox(double* lower, double* upper) const
{
    int i;

    for(i = 0; i < DIM; ++i)
    {
        lower[i] = ElementsBegin().corner(0).coords()[i];
        upper[i] = ElementsBegin().corner(0).coords()[i];
    }

    for(Element el = ElementsBegin(); el != ElementsEnd(); ++el)
    {
        double ellw[3], elup[3];
        el.BoundingBox(ellw, elup);

        for(i = 0; i < DIM; ++i)
        {
            lower[i] = std::min(lower[i], ellw[i]);
            upper[i] = std::max(upper[i], elup[i]);
        }
    }
}

double Mesh::MaxElementDiameter() const
{
    double diam, d;
    int    i, j, k;

    diam = 0.0;
    for(Element el = ElementsBegin(); el != ElementsEnd(); ++el)
    {
        d = 0.0;
        for(k = 0; k < el.ncorners(); ++k)
            for(j = 0; j < el.ncorners(); ++j)
                for(i = 0; i < DIM; ++i)
                    d = std::max(d, fabs(el.corner(k).coords()[i]-el.corner(j).coords()[i]));
        
        diam = std::max(diam, d);
    }

    return diam;
}

