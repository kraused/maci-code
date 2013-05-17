
// vi: tabstop=4:expandtab

#ifndef MACI_L2_TRANSFER_HH_INCLUDED
#define MACI_L2_TRANSFER_HH_INCLUDED 1

#include "L2Assembler.hh"
#include "TransferOperator.hh"
#include "HandshakeGeo.hh"

/// Forwarding
class Maci;


/// L2Transfer: Scale transfer using an L2 projection
class L2Transfer : public TransferOperator
{
 
public:
    /// Create a new instance
    /// @param[in] app  pointer to the Maci application instance
    /// @param[in] h    patch sizes (same for all particles)
    L2Transfer(Maci* app, const float h[]);

};

#endif

