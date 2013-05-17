
// vi: tabstop=4:expandtab

#ifndef MACI_INTERPOLATION_ASSEMBLER_STORAGE_REQ_QUERY_HH_INCLUDED
#define MACI_INTERPOLATION_ASSEMBLER_STORAGE_REQ_QUERY_HH_INCLUDED 1

#include "InterpolationAssemblerQueryBase.hh"
#include "DesignByContract.hh"
#include "ElementShadow.hh"
#include "QuadratureCellList.hh"

#include "pulib/pulib.hh"

/// Computes the storage requirements for the interpolation matrix
class InterpolationAssemblerStorageReqQuery : public InterpolationAssemblerQueryBase
{
    typedef InterpolationAssemblerQueryBase Base;
    typedef Base::Box Box;

public:
    InterpolationAssemblerStorageReqQuery(const ElementShadowList::Iterator element,
                   int *n)
    : Base(element), ntotal(n)
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
        if(Base::box.isInside(pt, Base::M_tol)) 
            (*ntotal) += Base::box.ncorners();
    }

private:
    /// The total number of entries that the InterpolationAssemblerQuery will insert
    int *ntotal;
};

#endif

