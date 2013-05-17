
// vi: tabstop=4:expandtab

#include "LsLumpedMeshMassAssembler.hh"
#include "HandshakeGeo.hh"
#include "NodeList.hh"


LsLumpedMeshMassAssembler::LsLumpedMeshMassAssembler(HandshakeGeo* handshake, Assembler* lsTransferAssembler)
: handshake(handshake), lsTransferAssembler(lsTransferAssembler)
{

}

void LsLumpedMeshMassAssembler::assemble()
{
    int64 idx;
    long  i;

    /// Q: Do we need this?
    for(NodeList::ConstIterator it = handshake->nodes().begin(); it != handshake->nodes().end(); ++it)
        if(gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(*it))
        {
            idx = Int64::concat(gcpe()->world.myrank(), fe::interface::Node::Index(*gcpe()->GetFESolver(), *it));
            matrix.insert(idx, idx, 0.);
        }
    matrix.sort();

    /// Lump the transfer operator
    i = 0;
    for(AssemblerOutputMatrix::Iterator a = lsTransferAssembler->matrix.begin(); a != lsTransferAssembler->matrix.end(); ++a)
    {
        matrix.insert(a->row, a->row, a->val);

        if((i++) % 1000 == 0)
            matrix.SumupDuplicates();
    }
    matrix.SumupDuplicates();
}

