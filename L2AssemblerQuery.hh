
// vi: tabstop=4:expandtab

#ifndef MACI_L2_ASSEMBLER_QUERY_HH_INCLUDED
#define MACI_L2_ASSEMBLER_QUERY_HH_INCLUDED 1

#include "TreeAssemblerIntermediateFormat.hh"
#include "L2AssemblerQueryBase.hh"
#include "DesignByContract.hh"
#include "ElementShadow.hh"
#include "QuadratureCellList.hh"
#include "PUSpace.hh"
#include "ShapeFunction.hh"

#include "quad/GaussPointList.hh"

#include "fe/FiniteElements.hh"

/// Assembles the scale transfer matrix for the L2 projection
class L2AssemblerQuery : public L2AssemblerQueryBase
{
    typedef L2AssemblerQueryBase Base;
    typedef quad::GaussPointList<double, DIM> GaussPointList;
    typedef quad::GaussPoint<double, DIM> GaussPoint;
    typedef Base::Box Box;
    typedef Base::QuadratureCell QuadratureCell;

public:
    L2AssemblerQuery(ElementShadowList::Iterator element,
             const QuadratureCellList& list, 
             TreeAssemblerIntermediateFormat<double> *matrix,
             const PUSpace* puspace)
    : Base(element, list), mat(matrix), X(puspace)
    {
    }

    bool matches(const double* pt, const Box& B, const pulib::Patch<DIM>& obj)
    {
        return Base::box.hasIntersection(obj, 1e-3*Base::M_tol);
    }
    
    /// Evaluate the shape function
    inline double theta(int i, const Box& b, const double* x)
    {
        double z[3];
        for(int d = 0; d < DIM; ++d) {
            z[d] = (x[d] - b.getLower()[d])/b.getSidelength(d);
        }
        
        return ShapeFunction(b.ncorners(), i, z);
    }

    /// Evaluate the partition of unity function
    inline double phi(const pulib::Patch<DIM>& obj, const double* x)
    {
        /// TODO The call to getFunction requires a lookup in the
        ///  hashmap which can be quiet expensive
        return PUSpace::value(*X->getFunction(obj.getId()),x);
    }   

    void process(const double* pt, const Box& B, const pulib::Patch<DIM>& obj)
    {
        for(QuadratureCell cell = Base::cells.begin(obj); cell != Base::cells.end(obj); ++cell) 
        {
            Box cut = cell->getIntersection(Base::box, Base::M_tol);
            if(0 == cut.getVolume())
                continue;

            // In general we would triangulate or tetrahedralize at
            // this point
        
            GaussPointList gp(1, cut);

            for(int i = 0; i < Base::box.ncorners(); ++i) 
            {
                double R = 0.;
                
                for(GaussPoint qt = gp.begin(); qt != gp.end(); ++qt)
                    R += theta(i, Base::box, qt.global())*phi(obj, qt.global())*qt.weight();
                
                CHECK_DEBUG(R >= -1e-7, "Check precision or algorithm");

                if(R > 1e-7)
                    mat->insert(Base::el, i, X->globalToLocal(obj.getId()), R);
            }
        }
    }

private:
    /// The intermadiate storage format
    TreeAssemblerIntermediateFormat<double> *mat;
    /// The partition of unity space
    const PUSpace* X;
};

#endif

