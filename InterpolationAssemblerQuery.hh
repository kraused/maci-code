
// vi: tabstop=4:expandtab

#ifndef MACI_INTERPOLATION_ASSEMBLER_QUERY_HH_INCLUDED
#define MACI_INTERPOLATION_ASSEMBLER_QUERY_HH_INCLUDED 1

#include "TreeAssemblerIntermediateFormat.hh"
#include "InterpolationAssemblerQueryBase.hh"
#include "DesignByContract.hh"
#include "ElementShadow.hh"
#include "QuadratureCellList.hh"
#include "PUSpace.hh"
#include "ShapeFunction.hh"

#include "fe/FiniteElements.hh"

#include <valarray>

/// Assembles the transposed interpolation matrix
class InterpolationAssemblerQuery : public InterpolationAssemblerQueryBase
{
    typedef InterpolationAssemblerQueryBase Base;
    typedef Base::Box Box;

public:
    InterpolationAssemblerQuery(ElementShadowList::Iterator element,
             TreeAssemblerIntermediateFormat<double> *matrix,
             const PUSpace* puspace)
    : Base(element), mat(matrix), X(puspace)
    {
    }

    bool matches(const double* pt, const Box& B, const pulib::Patch<DIM>& obj)
    {
        return Base::box.hasIntersection(obj, 1e-3*Base::M_tol);
    }
    
    void process(const double* pt, const Box& B, const pulib::Patch<DIM>& obj)
    {
        if(not Base::box.isInside(pt, Base::M_tol))
            return;

        double z[3], N;
        int    i, lid;

        for(i = 0; i < DIM; ++i)
            z[i] = (pt[i] - Base::box.getLower()[i])/Base::box.getSidelength(i);

        for(i = 0; i < Base::box.ncorners(); ++i) 
        {
            N   = ShapeFunction(Base::box.ncorners(), i, z);
            lid = X->globalToLocal(obj.getId());

            mat->insert(Base::el, i, lid, N);
        }
    }

private:
    /// The intermadiate storage format
    TreeAssemblerIntermediateFormat<double> *mat;
    /// The partition of unity space
    const PUSpace* X;
};

#endif

