
// vi: tabstop=4:expandtab

#ifndef MACI_LS_TRANSFER_HH_INCLUDED
#define MACI_LS_TRANSFER_HH_INCLUDED 1

#include "LsAssembler.hh"
#include "TransferOperator.hh"
#include "HandshakeGeo.hh"

/// Forwarding
class Maci;


class LsTransfer : public TransferOperator
{
 
public:
    /// Create a new instance
    /// @param[in]  app the Maci application instance pointer
    LsTransfer(Maci* app);

};

#endif

