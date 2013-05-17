
// vi: tabstop=4:expandtab

#include "LsTransfer.hh"


LsTransfer::LsTransfer(Maci* app)
: TransferOperator(app)
{
    AddPrimaryAssembler("LsTransfer", new LsAssembler(*handshake));
    CreateMatrix();
}

