
// vi: tabstop=4:expandtab

#ifndef MACI_PARTICLELIST_HH_INCLUDED
#define MACI_PARTICLELIST_HH_INCLUDED

#include <vector>

#include "ProcElement.hh"
#include "DesignByContract.hh"
#include "ParticleListSelector.hh"

#include "md/MolecularDynamics.hh"


/// ParticleList: A list of particles
class ParticleList : public std::vector<md::interface::ParticleHandle>
{

public:
    typedef md::interface::ParticleHandle ParticleHandle;
    typedef std::vector<ParticleHandle> Base;

public:
    /// Create an empty list
    ParticleList();
    
    /// Create a list and insert particles based on a selector
    template<typename Selector>
    ParticleList(const Selector& f)
    : Base()
    {
        AddSelection(f);
    }

    /// Add a new particle
    /// @param ph   the particle to add
    void AddParticle(const ParticleHandle& ph);

    typedef Base::iterator Iterator;
    typedef Base::const_iterator ConstIterator;

    /// Add all particles between [begin, end) where the function @f
    /// evaluates to true
    /// The functor selector class Selector should implement the function
    /// operator(ParticleHandle&) const
    /// Moreover it should implement begin() and end() functions where
    /// initializations (particle transformations, ...) are made
    template<typename Selector>
    void AddSelection(const ParticleHandle& begin, const ParticleHandle& end, const Selector& f)
    {
        CHECK_PRECOND_ALWAYS(PROC_MD == gcpe()->mytype);
        
        f.begin();  
        for(ParticleHandle ph = begin; ph != end; ++ph) 
        {
            if(f(ph))
                AddParticle(ph);
        }
        f.end();
    }

    /// Add all particles where the functor f evaluates to true
    /// @param f    the functor
    template<typename Selector>
    void AddSelection(const Selector& f)
    {
        CHECK_PRECOND_ALWAYS(PROC_MD == gcpe()->mytype);
        AddSelection(gcpe()->GetMMSystem()->begin(),
                     gcpe()->GetMMSystem()->end(), f);
    }

    /// Insert all particles into the list
    void fill()
    {
        for(ParticleHandle ph = gcpe()->GetMMSystem()->begin(); ph != gcpe()->GetMMSystem()->begin(); ++ph)
            AddParticle(ph);
    }

    /// Compute the total kinetic energy of the particles in the list
    double kineticEnergy();

    /// Compute the total potential energy of the list particles
    double potentialEnergy();
};

#endif


