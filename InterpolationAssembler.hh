
// vi: tabstop=4:expandtab

#ifndef MACI_INTERPOLATION_ASSEMBLER_HH_INCLUDED
#define MACI_INTERPOLATION_ASSEMBLER_HH_INCLUDED 1

#include <valarray>

#include "Assembler.hh"
#include "TreeAssembler.hh"
#include "InterpolationAssemblerStorageReqQuery.hh"
#include "InterpolationAssemblerQuery.hh"
#include "DesignByContract.hh"
#include "ParticleList.hh"

#include "linalg/CoordMatrix.hh"


/// InterpolationAssembler: Assembler for the interpolation matrix between
///                         the mesh and particles
class InterpolationAssembler : public TreeAssembler
{

public:
    /// Create a new instance
    /// @param hs   the handshake geometry
    InterpolationAssembler(const HandshakeGeo& hs);

    /// Destroy
    ~InterpolationAssembler();

    /// @overrides
    void assemble();

    /// @overrides
    long QueryStorageRequirements(ElementShadowList::Iterator el)
    {
        int n;
        InterpolationAssemblerStorageReqQuery q(el, &n);

        TreeAssembler::puspace.queryTreeLocal(q);
        return n;
    }

    /// @overrides
    void BuildIntermediateMatrix(ElementShadowList::Iterator el, TreeAssemblerIntermediateFormat<double> *mat)
    {
        InterpolationAssemblerQuery q(el, mat, &(TreeAssembler::puspace));
        TreeAssembler::puspace.queryTreeLocal(q);
    }

    /// @overrides
    void HandleDuplicates(linalg::CoordMatrix<int64, double>& mat)
    {
        /// Remove duplicates
        mat.removeDuplicates();
    }

};

#endif

