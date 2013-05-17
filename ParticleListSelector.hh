
// vi: tabstop=4:expandtab

#ifndef MACI_PARTICLELIST_SELECTOR_HH_INCLUDED
#define MACI_PARTICLELIST_SELECTOR_HH_INCLUDED

#include "ProcElement.hh"
#include "md/MolecularDynamics.hh"

#include "polylib/polylib.hh"

/// Base class for all particle selectors
class ParticleListSelector
{

public:
    /// Create a new instance
    /// @warning    This function will fail on FE processors
    ParticleListSelector();

protected:
    /// reference to the md system
    md::interface::System& sys;
};

/// Select all particles
class SelectAllParticles : public ParticleListSelector
{

public:
    inline void begin() const
    {
    }

    inline void end  () const
    {
    }

    inline bool operator()(const md::interface::ParticleHandle& ph) const
    {
        return true;
    }

};

/// A selector functor which allows for choosing particles which initial position
/// in the handshake region
class SelectHandshakeParticles : public ParticleListSelector
{

public:

    /// Creates a piggyback access epoche
    void begin() const;

    /// Ends the epoche
    void end() const;

    inline bool operator()(const md::interface::ParticleHandle& ph) const
    {
        using namespace md::interface;
        return (-1 != Property::Piggyback(this->sys, ph)->bdIndex);
    }
};

/// Select particles outside the handshake region
class SelectNonHandshakeParticles : public ParticleListSelector
{

public:

    /// Creates a piggyback access epoche
    void begin() const;

    /// Ends the epoche
    void end() const;

    inline bool operator()(const md::interface::ParticleHandle& ph) const
    {
        using namespace md::interface;
        return (-1 == Property::Piggyback(this->sys, ph)->bdIndex);
    }
};

/// Select particles inside a box
class SelectParticlesInBox : public ParticleListSelector
{

public:

public:
    /// Create a new instance
    /// @warning    This function will fail on FE processors
    SelectParticlesInBox(const polylib::Box<double,DIM>& B);

    /// Creates a piggyback access epoche
    void begin() const;

    /// Ends the epoche
    void end() const;

    inline bool operator()(const md::interface::ParticleHandle& ph) const
    {
        using namespace md::interface;

        double pos[3];
        for(int d = 0; d < 3; ++d) {
            pos[d] = Property::RefP(this->sys, ph, d);
        }

        return box.isInside(pos,1e-7*box.getVolume());
    }

private:
    /// The box
    polylib::Box<double,DIM> box;
};

#endif

