
// vi: tabstop=4:expandtab

#include "md/tremolo/ParticleHandle.hh"
#include "md/tremolo/System.hh"

using namespace md::interface;

ParticleHandle::ParticleHandle(const System& sys)
: _p(0), idx(0), _sys(sys)
{
    std::fill(_ijk, _ijk+3, 0);
}

ParticleHandle::ParticleHandle(const System& sys, ParticlePtr p, int ii, const int* ijk)
: _p(p), idx(ii), _sys(sys)
{
    if(ijk)
        std::copy(ijk, ijk+3, _ijk);
    else
        std::fill(_ijk, _ijk+3, 0);
} 

ParticleHandle::ParticleHandle(const System& sys, ParticlePtr p, int ii, int i, int j, int k)
: _p(p), idx(ii), _sys(sys)
{
    _ijk[0] = i;
    _ijk[1] = j;
    _ijk[2] = k;
}

ParticleHandle& ParticleHandle::operator++()
{
    if(0 == (_p = _p->next)) 
        _p = _sys.NextParticle(&_ijk);
    ++idx;

    return *this;
}

