
// vi: tabstop=4:expandtab

#include "DesignByContract.hh"
#include "PMLFineFluctuationDamper.hh"
#include "ProcElement.hh"
#include "DesignByContract.hh"
#include "DampingCoefficientsComputer.hh"
#include "MexicoFunctors.hh"

#include "md/MolecularDynamics.hh"


PMLFineFluctuationDamper::PMLFineFluctuationDamper(Maci* app, FineFluctuationFilter* Q, const Function<double,3>* D)
: DampingMethod(app), Q(Q)
{
    if(PROC_MD == gcpe()->mytype)
    {
        DampingCoefficientsComputer::ComputeCoefficients(D);
        DampingCoefficientsComputer::WriteCoefficients();
    }
}


/// PMLFineFluctuationDamper_Functor1: Functor for the computation of the first (velocity) term in
///                                    the PML
struct PMLFineFluctuationDamper_Functor1 : public MexicoFunctor_i3o3, public MexicoFunctor_2b_fb, public MexicoFunctor_V
{

    /// Constructor
    PMLFineFluctuationDamper_Functor1(std::valarray<double>* F)
    : F(F)
    {
    }

    void scat(md::interface::System* system, md::interface::ParticleHandle& ph, int k, double* buf) const
    {
        using md::interface::Property;
        CHECK_DEBUG(0 == k, 0);

        double m, d;

        m = Property::IntegratorMass(*system, ph);
        d = Property::Piggyback(*system, ph)->dampingD;

        (*F)[3*ph.GetLocalId()+0] = -m*d*2*buf[0];
        (*F)[3*ph.GetLocalId()+1] = -m*d*2*buf[1];
        (*F)[3*ph.GetLocalId()+2] = -m*d*2*buf[2];    
    }

    /// Pointer to the force array
    std::valarray<double>* F;

};

/// PMLFineFluctuationDamper_Functor2: Functor for the computation of the second (displacement) term
///                                    in the PML
struct PMLFineFluctuationDamper_Functor2 : public MexicoFunctor_i3o3, public MexicoFunctor_2b_fb, public MexicoFunctor_U
{

    /// Constructor
    PMLFineFluctuationDamper_Functor2(std::valarray<double>* F)
    : F(F)
    {
    }

    void scat(md::interface::System* system, md::interface::ParticleHandle& ph, int k, double* buf) const
    {
        using md::interface::Property;
        CHECK_DEBUG(0 == k, 0);

        double m, d;

        m = Property::IntegratorMass(*system, ph);
        d = Property::Piggyback(*system, ph)->dampingD;

        (*F)[3*ph.GetLocalId()+0] -= m*d*d*buf[0];
        (*F)[3*ph.GetLocalId()+1] -= m*d*d*buf[1];
        (*F)[3*ph.GetLocalId()+2] -= m*d*d*buf[2];
    }

    /// Pointer to the force array
    std::valarray<double>* F;

};

void PMLFineFluctuationDamper::ComputeDissipativeForce(std::valarray<double>* F)
{
    using namespace md::interface;
    CHECK_PRECOND_ALWAYS(PROC_MD == gcpe()->mytype);

    /* The list is created before we call AccessBegin() because the selector
       currently (as of r447) creates its own epoch and nesting is currently
       not supported by the MD interface.
     */
    SelectHandshakeParticles selector;
    ParticleList p(selector);

    system->AccessBegin(System::RD_POS   |
                        System::RD_REFP  |
                        System::RD_VEL   |
                        System::RD_PIGGYBACK);

    Q->compute(p, PMLFineFluctuationDamper_Functor1(F));
    Q->compute(p, PMLFineFluctuationDamper_Functor1(F));

    system->AccessEnd();
}

