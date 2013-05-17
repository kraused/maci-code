
// vi: tabstop=4:expandtab

#ifndef MACI_ATOM_WEIGHTED_MASS_ASSEMBLER_HH_INCLUDED
#define MACI_ATOM_WEIGHTED_MASS_ASSEMBLER_HH_INCLUDED 1

#include "ParticleList.hh"
#include "Assembler.hh"


/// AtomWeightedMassAssembler: Assembles the weighted mass
///                            matrix for a list of particles
class AtomWeightedMassAssembler : public Assembler
{

public:
    /// Create a new instance
    AtomWeightedMassAssembler(const ParticleList& list);

    /// @override
    void assemble();

private:
    ParticleList list;

};

#endif

