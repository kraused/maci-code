
// vi: tabstop=4:expandtab

#include "ugTimeIntegrator.hh"
#include "TauProfiling.hh"


UG_IF::TimeIntegrator::TimeIntegrator(SolverImplementation *solver, double tau, ::TimeIntegrator* base)
: solver(solver), base(base)
{
    dt  =     tau;
    dtf = 0.5*tau;
}

void UG_IF::TimeIntegrator::verlet_kick()
{
    int i;

    TAU_PROFILE("TimeIntegrator::verlet_kick", "", TAU_DEFAULT);

    for(fe::ug::NodePointer<DIM> nd = solver->GetMesh()->NodesBegin(); nd != solver->GetMesh()->NodesEnd(); ++nd)
        for(i = 0; i < DIM; ++i)
            solver->V(nd,i) += dtf/solver->M(nd)*solver->F(nd,i);
}

void UG_IF::TimeIntegrator::verlet_drift()
{
    int i;

    TAU_PROFILE("TimeIntegrator::verlet_drift", "", TAU_DEFAULT);

    for(fe::ug::NodePointer<DIM> nd = solver->GetMesh()->NodesBegin(); nd != solver->GetMesh()->NodesEnd(); ++nd)
        for(i = 0; i < DIM; ++i)        
            solver->U(nd,i) += dt*solver->V(nd,i);
}

void UG_IF::TimeIntegrator::verlet_kick_drift()
{
    int i;

    TAU_PROFILE("TimeIntegrator::verlet_kick_drift", "", TAU_DEFAULT);

    for(fe::ug::NodePointer<DIM> nd = solver->GetMesh()->NodesBegin(); nd != solver->GetMesh()->NodesEnd(); ++nd)
    {
        for(i = 0; i < DIM; ++i)
            solver->V(nd,i) += dtf/solver->M(nd)*solver->F(nd,i);
        for(i = 0; i < DIM; ++i)
            solver->U(nd,i) += dt*solver->V(nd,i);
    }
}

