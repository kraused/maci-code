
// vi: tabstop=4:expandtab

#include "NoopDamper.hh"


NoopDamper::NoopDamper(Maci* app)
: DampingMethod(app)
{
}

void NoopDamper::ComputeDissipativeForce(std::valarray<double>* F)
{
}

