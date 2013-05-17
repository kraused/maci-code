
// vi: tabstop=4:expandtab

#ifndef MACI_L2_ASSEMBLER_STORAGE_REQ_QUERY_HH_INCLUDED
#define MACI_L2_ASSEMBLER_STORAGE_REQ_QUERY_HH_INCLUDED 1

#include "L2AssemblerQueryBase.hh"
#include "DesignByContract.hh"
#include "ElementShadow.hh"
#include "QuadratureCellList.hh"

#include "pulib/pulib.hh"

/// Computes the storage requirements for the L2 scale transfer matrix
class L2AssemblerStorageReqQuery : public L2AssemblerQueryBase
{
    typedef L2AssemblerQueryBase Base;
    typedef Base::Box Box;
    typedef Base::QuadratureCell QuadratureCell;

public:
    L2AssemblerStorageReqQuery(const ElementShadowList::Iterator element,
                   const QuadratureCellList& list,
                   int *n)
    : Base(element, list), ntotal(n)
    {
        CHECK_PRECOND_ALWAYS(ntotal);
        *ntotal = 0;
    }

    bool matches(const double* pt, const Box& B, const pulib::Patch<DIM>& obj)
    {
        return Base::box.hasIntersection(obj, 1e-3*Base::M_tol);
    }

    void process(const double* pt, const Box& B, const pulib::Patch<DIM>& obj)
    {
        for(QuadratureCell cell = Base::cells.begin(obj); cell != Base::cells.end(obj); ++cell) 
        {
            Box cut = cell->getIntersection(Base::box, Base::M_tol);
            if(0 == cut.getVolume())
                continue;

            (*ntotal) += Base::box.ncorners();
        }
    }

private:
    /// The total number of entries that the L2AssemblerQuery will insert
    int *ntotal;
};

#endif

