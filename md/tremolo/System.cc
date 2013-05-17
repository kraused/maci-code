
// vi: tabstop=4:expandtab

#include "md/tremolo/System.hh"
#include "md/Writer.hh"

#include <assert.h>


using namespace md::interface;


System::System(Implementation& Imp)
: Base(Imp), epoch_type(INVALID), writer(0)
{
}


System::~System()
{
    if(writer)
        delete writer;
}


ParticleHandle System::begin() const
{
    const Implementation *Imp = this->GetImplementation();
    int     ijk[3];
    ParticlePtr p = 0;

    ijk[0] = Imp->iborder(tremolo::LOW)[0];
    ijk[1] = Imp->iborder(tremolo::LOW)[1];
    ijk[2] = Imp->iborder(tremolo::LOW)[2];

    if(0 == (p = (ParticlePtr )(*Imp)(ijk[0], ijk[1], ijk[2]))) 
        p = NextParticle(&ijk);
    
#ifndef NDEBUG
    // If we pretend that there are no particles then at least we should
    // have looked at all linked cells
    if(0 == p)
    {
        ASSERT(ijk[0] == Imp->iborder(tremolo::HIGH)[0]-1);
        ASSERT(ijk[1] == Imp->iborder(tremolo::HIGH)[1]-1);
        ASSERT(ijk[2] == Imp->iborder(tremolo::HIGH)[2]-1);
        
        ASSERT(ParticleHandle(*this, p, -1, ijk) == end());

        const Particle *q;
        int     cnt = 0;
        particles_for_each(Imp, q) {
            ++cnt;
        }

        ASSERT(0 == cnt);
    }
#endif

    return ParticleHandle(*this, p, 0, ijk);
}

ParticleHandle System::end() const
{
    const Implementation *Imp = this->GetImplementation();

    // Don't use NlocalAtoms() here because this has complexit O(N)
    return ParticleHandle(*this, 0, -1/*NlocalAtoms()*/,
                Imp->iborder(tremolo::HIGH)[0]-1,
                Imp->iborder(tremolo::HIGH)[1]-1,
                Imp->iborder(tremolo::HIGH)[2]-1);
}

ParticlePtr System::NextParticle(int (*ijk)[3]) const
{
    const Implementation *Imp = this->GetImplementation();
    ParticlePtr p = 0;

    if((*ijk)[2] != Imp->iborder(tremolo::HIGH)[2]-1) {
        (*ijk)[2] += 1;

        if(!(p = (ParticlePtr )(*Imp)((*ijk)[0], (*ijk)[1], (*ijk)[2]))) { 
            // Call NextParticle with index NULL because we do not
            // want to increment the counter twice
            p = NextParticle(ijk); 
        }
    } else if((*ijk)[1] != Imp->iborder(tremolo::HIGH)[1]-1) {
        (*ijk)[1] += 1;
        (*ijk)[2]  = Imp->iborder(tremolo::LOW)[2];

        if(!(p = (ParticlePtr )(*Imp)((*ijk)[0], (*ijk)[1], (*ijk)[2]))) { 
            p = NextParticle(ijk);
        }
    } else if((*ijk)[0] != Imp->iborder(tremolo::HIGH)[0]-1) {
        (*ijk)[0] += 1;
        (*ijk)[1]  = Imp->iborder(tremolo::LOW)[1];
        (*ijk)[2]  = Imp->iborder(tremolo::LOW)[2];

        if(!(p = (ParticlePtr )(*Imp)((*ijk)[0], (*ijk)[1], (*ijk)[2]))) { 
            p = NextParticle(ijk);
        }
    }

    return p;
}

void System::GetSpatialRegion(double* lower, double* upper) const
{
    const Implementation *Imp = this->GetImplementation();

    for(int d = 0; d < 3; ++d) {
        lower[d] = Imp->problem_instance.LCS.LocalBorder[2*d  ]* \
               Imp->problem_instance.LCS.HCellRCut[d] 
                + Imp->shift[d];
        upper[d] = Imp->problem_instance.LCS.LocalBorder[2*d+1]* \
               Imp->problem_instance.LCS.HCellRCut[d]  
                + Imp->shift[d];
    }
}

void System::Write(const std::string& file)
{
    if(0 == writer)
        writer = Writer::New(this);
        ASSERT(writer);

        writer->Write(file, this);
}       

