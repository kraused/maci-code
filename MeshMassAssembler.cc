
// vi: tabstop=4:expandtab

#include "MeshMassAssembler.hh"
#include "ElementBoundingBox.hh"
#include "ShapeFunction.hh"

#include "quad/GaussPointList.hh"


void MeshMassAssembler::AssembleOnElement(const Element& el, const Function<double, 3>& f, double* localMat)
{
    int i, j, NC;
    
    NC = el.ncorners();

    /// TODO  Since the quad module currently only supports quadrature on cubes
    ///       we pass the bounding box of el to the constructor of the list of
    ///       quadrature points. This is fine if the element's bounding box coincides
    ///       with the element. THIS IS AN ASSUMPTION!
    /// TODO  We need higher order quadrature rules
    quad::GaussPointList<double, DIM> gpoints(1, ElementBoundingBox(el));

    for(i = 0; i < NC; ++i)
        for(j = 0; j < NC; ++j)
        {
            localMat[i*NC + j] = 0.0;
            for(quad::GaussPoint<double, DIM> gp = gpoints.begin(); gp != gpoints.end(); ++gp)
                localMat[i*NC + j] += gp.weight()*f.val(gp.global())*ShapeFunction(NC, i, gp.local())*ShapeFunction(NC, j, gp.local());
        }
}


