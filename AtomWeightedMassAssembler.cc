
// vi: tabstop=4:expandtab

#include "AtomWeightedMassAssembler.hh"


AtomWeightedMassAssembler::AtomWeightedMassAssembler(const ParticleList& list)
: list(list)
{
}

void AtomWeightedMassAssembler::assemble()
{
    int i;
    int myrank = gcpe()->world.myrank();

    if(0 == list.size())
        goto fn_exit;

    using md::interface::System;
    using md::interface::Property;
    System* sys;

    sys = gcpe()->GetMMSystem();
    sys->AccessBegin(System::RD_MASS |
                     System::RD_PIGGYBACK);

    i = 0;
    for(ParticleList::ConstIterator it = list.begin(); it != list.end(); ++it, ++i)
        matrix.insert(Int64::concat(myrank,i), Int64::concat(myrank,i), Property::WeightedMass(*sys, *it));

    sys->AccessEnd();

fn_exit:
    return;
}

