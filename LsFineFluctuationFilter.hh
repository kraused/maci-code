
// vi: tabstop=4:expandtab

#ifndef MACI_LS_FINE_FLUCTUATION_FILTER_HH_INCLUDED
#define MACI_LS_FINE_FLUCTUATION_FILTER_HH_INCLUDED 1

#include "FineFluctuationFilter.hh"
#include "HandshakeGeo.hh"

/// Forwarding
class Maci;


/// Fine fluctuation filter based on least squares projection
class LsFineFluctutationFilter : public FineFluctuationFilter
{
 
public:
    /// Create a new instance
    /// @param[in] app  the Maci application instance
    ///
    /// @warning Releases the memory used by the assembler. This makes sense
    ///      as one typically doesn't create more than one fluctuation
    ///      filter;
    ///
    LsFineFluctutationFilter(Maci* app);

};

#endif

