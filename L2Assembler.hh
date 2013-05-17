
// vi: tabstop=4:expandtab

#ifndef MACI_L2_ASSEMBLER_HH_INCLUDED
#define MACI_L2_ASSEMBLER_HH_INCLUDED 1

#include <string>

#include "Assembler.hh"
#include "TreeAssembler.hh"
#include "L2AssemblerStorageReqQuery.hh"
#include "L2AssemblerQuery.hh"
#include "DesignByContract.hh"

#include "linalg/CoordMatrix.hh"


/// L2Assembler: Assembler for the L2 projection scale-transfer
class L2Assembler : public TreeAssembler
{

public:
    /// Create a new instance
    /// @param hs   the handshake geometry
    /// @param h    the patch sizes (for each particle the same)
    L2Assembler(const HandshakeGeo& hs, const float h[]);

    /// Destroy
    ~L2Assembler();

    /// @overrides
    void assemble();

    /// @overrides
    long QueryStorageRequirements(ElementShadowList::Iterator el)
    {
        int n;
        L2AssemblerStorageReqQuery q(el, list, &n);
        
        TreeAssembler::puspace.queryTreeLocal(q);
        return n;
    }

    /// @overrides
    void BuildIntermediateMatrix(ElementShadowList::Iterator el, TreeAssemblerIntermediateFormat<double> *mat)
    {
        L2AssemblerQuery q(el, list, mat, &(TreeAssembler::puspace));
        TreeAssembler::puspace.queryTreeLocal(q);
    }

    /// @overrides
    void HandleDuplicates(linalg::CoordMatrix<int64, double>& mat)
    {
        /// Sum up multiple entries
        mat.SumupDuplicates();
    }

private:
    /// The quadrature cell list
    QuadratureCellList list;

};

/// Create a unique name for the assembler. The name must
/// include the PUM size h to make sure that multiple assembler
/// for different patch sizes can be created
std::string L2UniqueAssemblerName(const std::string& name, const float* h);

#endif

