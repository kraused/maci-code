
// vi: tabstop=4:expandtab

#include "RattleIntegrator.hh"
#include "NodeList.hh"
#include "Maci.hh"
#include "RattleCorrector.hh"
#include "Delete.hh"


RattleIntegrator::RattleIntegrator(Maci* app, DampingMethod* damper, RattleCorrector* corrector)
: TimeIntegrator(app)
{
    /// Important: Since we use the same name for the member
    ///            as for the argument (which is bad but looks
    ///            nicer) we need the this-> prefix!
    this->damper    = damper;
    this->corrector = corrector;

    if(PROC_FE == gcpe()->mytype)
    {
        lambda = new NodalValueBuffer<double>(NodeList(*this->handshake, SelectOwnedNodes()));
        mu     = new NodalValueBuffer<double>(NodeList(*this->handshake, SelectOwnedNodes()));
    }
}

RattleIntegrator::~RattleIntegrator()
{
    DeletePointer(lambda);
    DeletePointer(mu);
}

void RattleIntegrator::CorrectAfterKickDrift()
{
    ((RattleCorrector* )this->corrector)->CorrectAfterKickDrift(lambda);
}

void RattleIntegrator::CorrectAfterKick()
{
    ((RattleCorrector* )this->corrector)->CorrectAfterKick(mu);
}

