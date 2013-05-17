
// vi: tabstop=4:expandtab

#ifndef MACI_LS_ASSEMBLER_QUERY_HH_INCLUDED
#define MACI_LS_ASSEMBLER_QUERY_HH_INCLUDED 1

#include "TreeAssemblerIntermediateFormat.hh"
#include "LsAssemblerQueryBase.hh"
#include "DesignByContract.hh"
#include "ElementShadow.hh"
#include "QuadratureCellList.hh"
#include "PUSpace.hh"
#include "ShapeFunction.hh"

#include "fe/FiniteElements.hh"

#include <valarray>

/// Assembles the scale transfer matrix for the least squares projection
class LsAssemblerQuery : public LsAssemblerQueryBase
{
    typedef LsAssemblerQueryBase Base;
    typedef Base::Box Box;

public:
    LsAssemblerQuery(ElementShadowList::Iterator element,
             TreeAssemblerIntermediateFormat<double> *matrix,
             const PUSpace *puspace,
             const std::valarray<double>& masses)
    : Base(element), mat(matrix), X(puspace), m(masses)
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

        for(i = 0; i <DIM; ++i)
            z[i] = (pt[i] - Base::box.getLower()[i])/Base::box.getSidelength(i);

        for(i = 0; i < Base::box.ncorners(); ++i) 
        {
            N   = ShapeFunction(Base::box.ncorners(),i,z);
    
            /** @warning We assume that the ordering of particles
             *       in the partition of unity space (which is
             *       given by obj.getId() matches the index
             *       in the particle list (used to access the
             *       particle masses).
             */

            lid = X->globalToLocal(obj.getId());
            CHECK_PRECOND_ALWAYS(0 <= lid && lid < (int )m.size());

            if(m[lid]*N > 1e-12)
                mat->insert(Base::el, i, lid, m[lid]*N);
        }
    }

private:
    /// The intermadiate storage format
    TreeAssemblerIntermediateFormat<double> *mat;
    /// The partition of unity space
    const PUSpace *X;
    /// Atom masses
    const std::valarray<double>& m;
};

#endif


