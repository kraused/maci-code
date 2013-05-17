
// vi: tabstop=4:expandtab

#ifndef MACI_QUAD_GAUSS_POINT_PAYLOAD_HH_INCLUDED
#define MACI_QUAD_GAUSS_POINT_PAYLOAD_HH_INCLUDED 1

namespace quad
{

    /// GaussPointPayload: Class that bundles the actual data
    ///                    for the GaussPoint class
    ///
    /// Seperating the members of GaussPoint from the class itself
    /// makes the implementation of the latter class much easier 
    /// because the class only needs to manage a pointer to the payload
    /// (which is stored in the list) and can iterate over the payloads
    template<typename Tp>
    struct GaussPointPayload
    {
        /// The local coordinates
        Tp lpt[3];
        /// The global coordinates
        Tp gpt[3];
        /// The weight
        Tp wgt;
    };        

}

#endif

