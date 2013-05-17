
// vi: tabstop=4:expandtab

#include "AtomMassAssembler.hh"


AtomMassAssembler::AtomMassAssembler(const ParticleList& list)
: list(list)
{
}

void AtomMassAssembler::assemble()
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
        matrix.insert(Int64::concat(myrank,i), Int64::concat(myrank,i), Property::Mass(*sys, *it));

    sys->AccessEnd();

fn_exit:
    ;
}

