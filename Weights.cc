
// vi: tabstop=4:expandtab

#include "Weights.hh"
#include "ProcElement.hh"

#include "md/MolecularDynamics.hh"


void Weights::Update(ProcElement& here)
{
    using namespace md::interface;

    if(PROC_MD == gcpe()->mytype)
    {
        System* sys = here.GetMMSystem();

        sys->AccessBegin(System::WR_PIGGYBACK | System::RD_POS);
        for(ParticleHandle ph = sys->begin(); ph != sys->end(); ++ph) 
        {
            double pos[3];
            for(int i = 0; i < DIM; ++i)
                pos[i] = md::interface::Property::Pos(*sys, ph, i);
#if 2 == DIM
            pos[2] = 0.0;   // Shouldn't matter. Set it to zero to be sure
#endif

            Property::Piggyback(*sys,ph)->weight = val(pos);
        }
        sys->AccessEnd();

        WriteFile();
    }
}

void Weights::WriteFile()
{
    using namespace md::interface;
    CHECK_PRECOND_ALWAYS(PROC_MD == gcpe()->mytype);

    int i;

    System& sys = *gcpe()->GetMMSystem();
    sys.AccessBegin(System::WR_PIGGYBACK |
            System::RD_POS);

    double* wgt = new double[sys.NlocalAtoms()];
    Writer* w   = md::interface::Writer::New(&sys);

    i = 0;
    for(ParticleHandle ph = sys.begin(); ph != sys.end(); ++ph)
    {
        wgt[i++] = Property::Piggyback(sys, ph)->weight;
    }

    sys.AccessEnd();

    w->WriteArrayDouble("weight", &sys, wgt, "w");

    delete w;
    delete wgt;
}

