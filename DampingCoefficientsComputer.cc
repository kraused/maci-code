
// vi: tabstop=4:expandtab

#include "DesignByContract.hh"
#include "DampingCoefficientsComputer.hh"
#include "ProcElement.hh"
#include "DesignByContract.hh"

#include "md/MolecularDynamics.hh"

#ifdef HAVE_PVO
#include <pvo.h>
#endif

void DampingCoefficientsComputer::ComputeCoefficients(const Function<double,3>* D)
{
    using namespace md::interface;
    CHECK_PRECOND_ALWAYS(PROC_MD == gcpe()->mytype);
    CHECK_PRECOND_ALWAYS(D);

    System& sys = *gcpe()->GetMMSystem();
    sys.AccessBegin(System::WR_PIGGYBACK |
            System::RD_POS);

    for(ParticleHandle ph = sys.begin(); ph != sys.end(); ++ph) 
    {
        double pos[3];

        for(int i = 0; i < DIM; ++i) 
            pos[i] = md::interface::Property::Pos(sys, ph, i);
#if 2 == DIM
        pos[2] = 0.0;
#endif

        Property::Piggyback(sys, ph)->dampingD = D->val(pos);
    }
    
    sys.AccessEnd();
}

void DampingCoefficientsComputer::WriteCoefficients()
{
    using namespace md::interface;
    CHECK_PRECOND_ALWAYS(PROC_MD == gcpe()->mytype);

    int i;

    System& sys = *gcpe()->GetMMSystem();
    sys.AccessBegin(System::WR_PIGGYBACK |
            System::RD_POS);

    double* damp = new double[sys.NlocalAtoms()];
    Writer* w = md::interface::Writer::New(&sys);

    i = 0;
    for(ParticleHandle ph = sys.begin(); ph != sys.end(); ++ph)
    {
        damp[i++] = Property::Piggyback(sys, ph)->dampingD;
    }

    sys.AccessEnd();

    w->WriteArrayDouble("damping", &sys, damp, "D");
    
    delete w;
    delete[] damp;
}

