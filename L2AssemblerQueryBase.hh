
// vi: tabstop=4:expandtab

#ifndef MACI_L2_ASSEMBLER_QUERY_BASE_HH_INCLUDED
#define MACI_L2_ASSEMBLER_QUERY_BASE_HH_INCLUDED 1

#include "ElementBoundingBox.hh"
#include "DesignByContract.hh"
#include "ElementShadowList.hh"
#include "QuadratureCellList.hh"

#include "pulib/pulib.hh"


/// Base class for all queries used for the assembly of the scale transfer 
/// matrix for the L2 projection
class L2AssemblerQueryBase : public pulib::Query<DIM>
{

protected:
    typedef pulib::Query<DIM>::Box Box;
    typedef QuadratureCellList::ConstIterator QuadratureCell;

public:
    L2AssemblerQueryBase(ElementShadowList::Iterator element, 
                 const QuadratureCellList& list)
    : el(element), box(element->boundbox()), boxvol(box.getVolume()), 
      cells(list), M_tol(gcpe()->absTolerance()*gcpe()->charLength())
    {
        if(2 == DIM)
            CHECK_PRECOND_ALWAYS(4 == el->ncorners);
        if(3 == DIM)
            CHECK_PRECOND_ALWAYS(8 == el->ncorners);
    }

protected:
    /// The element shadow
    const ElementShadowList::Iterator el;
    /// The bounding box of the element
    Box box;
    /// The volume of the bounding box
    double boxvol;
    /// Quadrature element list
    const QuadratureCellList& cells;
    /// Tolerance value
    double  M_tol;
};

#endif

