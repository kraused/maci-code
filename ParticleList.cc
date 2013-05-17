
// vi: tabstop=4:expandtab

#include "ParticleList.hh"
#include "ParticleListSelector.hh"

ParticleList::ParticleList()
: Base()
{
}

void ParticleList::AddParticle(const ParticleHandle& ph)
{
    Base::push_back(ph);
}

/// A weighting functor for the energy evaluation
class EnergyWeightFunctor
{

public:
    /// Create a new instance
    EnergyWeightFunctor()
    : sys(*gcpe()->GetMMSystem())
    {
    }

    inline bool operator()(const ParticleList::ParticleHandle& ph) const
    {
        using namespace md::interface;
        return Property::Piggyback(sys, ph)->weight;
    }
    
private:
    /// reference to the md system
    md::interface::System& sys;
};

double ParticleList::kineticEnergy()
{
    return md::interface::KineticEnergy(*gcpe()->GetMMSystem(), begin(), end(), EnergyWeightFunctor());
}

double ParticleList::potentialEnergy()
{
    return md::interface::PotentialEnergy(*gcpe()->GetMMSystem(), begin(), end(), EnergyWeightFunctor()); 
}


