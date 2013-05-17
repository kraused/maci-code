
// vi: tabstop=4:expandtab

#ifndef MACI_ELEMENT_SHADOW_HH_INCLUDED
#define MACI_ELEMENT_SHADOW_HH_INCLUDED 1

#include <mpi.h>

#include "PlainOldData.hh"

#include "polylib/polylib.hh"
#include "fe/FiniteElements.hh"


/// ElementShadow: The shadow of an element contains only a subset of
///                the important information.
/// ElementShadow instances represent (as a kind of proxy) the original
/// elements on particle processing elements.
struct ElementShadow : public PlainOldData<ElementShadow>
{
    /// Maximal number of corners an element can have
    static const int MAX_CORNERS_OF_ELEMENT = fe::interface::MAX_CORNERS_OF_ELEMENT;

    /// Get the MPI datatype for the data exchange. The function is
    /// not thread safe and the committed datatype is not freed before
    /// MPI is finalized.
    static MPI_Datatype type();

    /// Compute the bounding box of the element
    polylib::Box<double,DIM> boundbox() const;

    /// Number of corners of the element. Together with the dimension, this
    /// uniquely identifies the type of the element
    int     ncorners;
    /// Corner coordinates of the element
    double  corner[MAX_CORNERS_OF_ELEMENT][3];
    /// The rank of the owner processing element in the world
    /// communicator
    int     owner;
    /// The local index on the owning processing element
    int     localIndex;
};

/// Initialize the element shadow elsh from the element el. We provide this as a
/// function rather than the constructor of ElementShadow because ElementShadow 
/// should be a PlainOldData type.
void InitializeElementShadow(const fe::interface::Element& el, int localIndex, ElementShadow* elsh);

#endif

