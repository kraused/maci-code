
// vi: tabstop=4:expandtab

#ifndef MACI_ATOM_MASS_ASSEMBLER_HH_INCLUDED
#define MACI_ATOM_MASS_ASSEMBLER_HH_INCLUDED 1

#include "ParticleList.hh"
#include "Assembler.hh"


/// AtomMassAssembler: Assembles the mass matrix for a
///                    list of particles
class AtomMassAssembler : public Assembler
{

public:
    /// Create a new instance
    AtomMassAssembler(const ParticleList& list);

    /// @override
    void assemble();

private:
    ParticleList list;

};

#endif

