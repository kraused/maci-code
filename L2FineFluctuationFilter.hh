
// vi: tabstop=4:expandtab

#ifndef MACI_L2_FINE_FLUCTUATION_FILTER_HH_INCLUDED
#define MACI_L2_FINE_FLUCTUATION_FILTER_HH_INCLUDED 1

#include "FineFluctuationFilter.hh"
#include "HandshakeGeo.hh"

/// Forwarding
class Maci;


/// Fine fluctuation filter based on L2 projection
class L2FineFluctutationFilter : public FineFluctuationFilter
{
 
public:
    /// Create a new instance
    /// @param[in] app  the Maci application instance
    /// @param[in] h    patch sizes (same for all particles)
    ///
    /// @warning Releases the memory used by the assembler. This makes sense
    ///      as one typically doesn't create more than one fluctuation
    ///      filter;
    ///
    L2FineFluctutationFilter(Maci* app, const float h[]);

};

#endif

