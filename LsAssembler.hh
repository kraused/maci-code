
// vi: tabstop=4:expandtab

#ifndef MACI_LS_ASSEMBLER_HH_INCLUDED
#define MACI_LS_ASSEMBLER_HH_INCLUDED 1

#include <valarray>

#include "Assembler.hh"
#include "TreeAssembler.hh"
#include "LsAssemblerStorageReqQuery.hh"
#include "LsAssemblerQuery.hh"
#include "DesignByContract.hh"
#include "ParticleList.hh"

#include "linalg/CoordMatrix.hh"


/// LsAssembler: Assembler for the least squares projection from the
///              particle to the finite element space
class LsAssembler : public TreeAssembler
{

public:
    /// Create a new instance
    /// @param hs   the handshake geometry
    LsAssembler(const HandshakeGeo& hs);

    /// Destroy
    ~LsAssembler();

    /// @overrides
    void assemble();

    /// @overrides
    long QueryStorageRequirements(ElementShadowList::Iterator el)
    {
        int n;
        LsAssemblerStorageReqQuery q(el, &n);

        TreeAssembler::puspace.queryTreeLocal(q);
        return n;
    }

    /// @overrides
    void BuildIntermediateMatrix(ElementShadowList::Iterator el, TreeAssemblerIntermediateFormat<double> *mat)
    {
        LsAssemblerQuery q(el, mat, &(TreeAssembler::puspace), m);
        TreeAssembler::puspace.queryTreeLocal(q);
    }

    /// @overrides
    void HandleDuplicates(linalg::CoordMatrix<int64, double>& mat)
    {
        /// Sum up multiple entries
        mat.removeDuplicates();
    }

private:
    /// The particle list
    ParticleList list;
    /// Array of atom masses
    std::valarray<double> m;

};

#endif

