
// vi: tabstop=4:expandtab

#ifndef MACI_ELEMENT_BOUNDING_BOX_HH_INCLUDED
#define MACI_ELEMENT_BOUNDING_BOX_HH_INCLUDED 1

#include "fe/FiniteElements.hh"

#include "polylib/polylib.hh"

/// Compute the bounding box of a finite element
/// @param el   the finite element
/// @returns    bounding box
inline polylib::Box<double,DIM> ElementBoundingBox(const fe::interface::Element& el)
{
    double lower[3], upper[3];
    el.BoundingBox(lower, upper);

    return polylib::Box<double,DIM>(lower, upper);
}

#endif

