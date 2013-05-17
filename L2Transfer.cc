
// vi: tabstop=4:expandtab

#include "L2Transfer.hh"


L2Transfer::L2Transfer(Maci* app, const float h[])
: TransferOperator(app)
{
    AddPrimaryAssembler(L2UniqueAssemblerName("L2Transfer", h), new L2Assembler(*handshake, h));
    CreateMatrix();
}

