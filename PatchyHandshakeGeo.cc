
// vi: tabstop=4:expandtab

#include "PatchyHandshakeGeo.hh"
#include "Maci.hh"

PatchyHandshakeGeo::PatchyHandshakeGeo(Maci* app)
: HandshakeGeo(app)
{
}

PatchyHandshakeGeo::~PatchyHandshakeGeo()
{
    for(std::vector<GeoPrimitive*>::iterator P = pr_.begin(); P != pr_.end(); ++P)
        delete *P;
}

