
// vi: tabstop=4:expandtab

#include "DesignByContract.hh"
#include "FrictionFineFluctuationDamper.hh"
#include "ProcElement.hh"
#include "DesignByContract.hh"
#include "DampingCoefficientsComputer.hh"
#include "MexicoFunctors.hh"

#include "md/MolecularDynamics.hh"


FrictionFineFluctuationDamper::FrictionFineFluctuationDamper(Maci* app, FineFluctuationFilter* Q, const Function<double,3>* D)
: DampingMethod(app), Q(Q)
{
    if(PROC_MD == gcpe()->mytype)
    {
        DampingCoefficientsComputer::ComputeCoefficients(D);
        DampingCoefficientsComputer::WriteCoefficients();
    }
}


/// FrictionFineFluctuationDamper_Functor: Functor for the computation of the first (velocity) term in
///                                        the PML
///
/// On OS X it is very important to give the functors unique names as the linker might
/// silently pick the wrong one!
struct FrictionFineFluctuationDamper_Functor : public MexicoFunctor_i3o3, public MexicoFunctor_2b_fb, public MexicoFunctor_V
{

    /// Constructor
    FrictionFineFluctuationDamper_Functor(std::valarray<double>* G)
    : F(G)
    {
    }

    void scat(md::interface::System* system, md::interface::ParticleHandle& ph, int k, double* buf) const
    {
        using md::interface::Property;
        CHECK_DEBUG(0 == k, 0);

        double m, d;

        m = Property::IntegratorMass(*system, ph);
        d = Property::Piggyback(*system, ph)->dampingD;

        CHECK_DEBUG((std::size_t )3*(ph.GetLocalId()+1) <= F->size(), 0);

        (*F)[3*ph.GetLocalId()+0] -= m*d*2*buf[0];
        (*F)[3*ph.GetLocalId()+1] -= m*d*2*buf[1];
        (*F)[3*ph.GetLocalId()+2] -= m*d*2*buf[2];    
    }

    /// Pointer to the force array
    std::valarray<double>* F;

};


void FrictionFineFluctuationDamper::ComputeDissipativeForce(std::valarray<double>* F)
{
    using namespace md::interface;
    CHECK_PRECOND_ALWAYS(PROC_MD == gcpe()->mytype);

    /* The list is created before we call AccessBegin() because the selector
       currently (as of r447) creates its own epoch and nesting is currently
       not supported by the MD interface.
      */
    SelectHandshakeParticles selector;
    ParticleList p(selector);

    system->AccessBegin(System::RD_VEL | System::RD_PIGGYBACK);
    Q->compute(p, FrictionFineFluctuationDamper_Functor(F));
    system->AccessEnd();
}

