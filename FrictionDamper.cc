
// vi: tabstop=4:expandtab

#include "DesignByContract.hh"
#include "FrictionDamper.hh"
#include "ProcElement.hh"
#include "DesignByContract.hh"
#include "DampingCoefficientsComputer.hh"

#include "md/MolecularDynamics.hh"


FrictionDamper::FrictionDamper(Maci* app, const Function<double,3>* D)
: DampingMethod(app)
{
    if(PROC_MD == gcpe()->mytype)
    {
        DampingCoefficientsComputer::ComputeCoefficients(D);
        DampingCoefficientsComputer::WriteCoefficients();
    }
}

void FrictionDamper::ComputeDissipativeForce(std::valarray<double>* F)
{
    using namespace md::interface;

    System* sys = gcpe()->GetMMSystem();
    double  m, d;

    sys->AccessBegin(System::RD_VEL   |
             System::RD_PIGGYBACK);

    for(ParticleHandle ph = sys->begin(); ph != sys->end(); ++ph)
    {
        CHECK_DEBUG(ph.GetLocalId() < sys->NlocalAtoms(), "Local particle index is too large");

        m = Property::IntegratorMass(*sys, ph);
        d = Property::Piggyback(*sys, ph)->dampingD;

        (*F)[3*ph.GetLocalId()+0] = -2*m*d*Property::Vel(*sys, ph, 0);
        (*F)[3*ph.GetLocalId()+1] = -2*m*d*Property::Vel(*sys, ph, 1);
        (*F)[3*ph.GetLocalId()+2] = -2*m*d*Property::Vel(*sys, ph, 2);
    }

    sys->AccessEnd();
}

