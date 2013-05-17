
// vi: tabstop=4:expandtab

#include "L2Assembler.hh"
#include "utils/strprintf.hh"


L2Assembler::L2Assembler(const HandshakeGeo& hs, const float h[])
: TreeAssembler(hs, h), list(TreeAssembler::puspace)
{
}

L2Assembler::~L2Assembler()
{
}

void L2Assembler::assemble()
{
    AssembleIntermediateMatrix();
    CreateConsistentCoordinateMatrix();
    DeleteIntermediateMatrix();
}

std::string L2UniqueAssemblerName(const std::string& name, const float* h)
{
#if 2 == DIM
    return utils::strprintf("%s:[%.3f, %.3f]", name.c_str(), h[0], h[1]);
#endif
#if 3 == DIM
    return utils::strprintf("%s:[%.3f, %.3f, %.3f]", name.c_str(), h[0], h[1], h[2]);
#endif
}

