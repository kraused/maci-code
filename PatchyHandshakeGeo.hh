
// vi: tabstop=4:expandtab

#ifndef MACI_PATCHYHANDSHAKEGEO_HH_INCLUDED
#define MACI_PATCHYHANDSHAKEGEO_HH_INCLUDED

#include "HandshakeGeo.hh"
#include "GeoPrimitive.hh"


/// A handshake region consisting of the union of a set of geometric primitives
class PatchyHandshakeGeo : public HandshakeGeo
{
/* for SWIG */public:
    typedef HandshakeGeo    Base;
    
public:
    /// Construct a new instance
    /// @param[in]  app the Maci application instance
    PatchyHandshakeGeo(Maci* app);
    
    /// Destroy the instance
    ~PatchyHandshakeGeo();
    
    /// @overrides
    inline bool Intersects(const HandshakeGeo::Box& b) const
    {
        for(std::vector<GeoPrimitive*>::const_iterator P = pr_.begin(); P != pr_.end(); ++P)
            if((*P)->Intersects(b))
                return true;

        return false;
    }
    
    /// @overrides
    inline bool IsInside(const double* p) const
    {
        for(std::vector<GeoPrimitive*>::const_iterator P = pr_.begin(); P != pr_.end(); ++P)
            if((*P)->IsInside(p))
                return true;

        return false;
    }
    
    /// Add a new primitive
    /// Returns a reference to @this so that you can concate arguments
    inline PatchyHandshakeGeo& AddNewPrimitive(const GeoPrimitive* P)
    { 
        pr_.push_back(P->Clone());
        return *this;
    }
    
    /// Returns the number of primitives
    inline int GetNumPrimitives()
    {
        return pr_.size();
    }
    
private:
    /// The list of primitives
    std::vector<GeoPrimitive*> pr_;
};

#endif

