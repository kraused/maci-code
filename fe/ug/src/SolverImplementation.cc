
// vi: tabstop=4:expandtab

#include "SolverImplementation.hh"
#include "Delete.hh"
#include "TauProfiling.hh"

#include "ugBorderExchange.hh"
#include "ugMetaForceCalculator.hh"

/* We need to ask explicitly for INT[N]_MIN and INT[N]_MAX
      to be defined */
#define __STDC_LIMIT_MACROS
#include <stdint.h>


using namespace fe::ug;


UG_IF::SolverImplementation::SolverImplementation(Mesh* M)
: mesh(M), u(*mesh, 3), v(*mesh, 3)/*, f(*mesh, 3)*/, umat(0), wmat(0), imat(0), forcecalc(new ForceCalc(*mesh, 3)), direnforcer(0), potecalc(0)
{
}

UG_IF::SolverImplementation::~SolverImplementation()
{
    delete forcecalc;

    DeletePointer(potecalc);
    DeletePointer(umat);
    DeletePointer(wmat);
}

/* Take the implementation of the parallel stuff from another
   file because it is so big!
 */
#include "SolverImplementationPar.cc"

double UG_IF::SolverImplementation::KineticEnergy()
{
    double K = 0.0;

    for(NodePointer n = mesh->NodesBegin(NodePointer::PRIOM); n != mesh->NodesEnd(); ++n)
#if 2 == DIM
        K += (*imat)(n,n)[0]*( u[n][0]*u[n][0] + u[n][1]*u[n][1] );
#else
        K += (*imat)(n,n)[0]*( u[n][0]*u[n][0] + u[n][1]*u[n][1] + u[n][2]*u[n][2] );
#endif
    K *= 0.5;

    MPI_Allreduce(MPI_IN_PLACE, &K, 1, MPI_DOUBLE, MPI_SUM, fe::ug::GetCommunicator());

    return K;
}

double UG_IF::SolverImplementation::PotentialEnergy()
{
    return potecalc->eval();
}

void UG_IF::SolverImplementation::CalculateForces(double currtime)
{
    TAU_PROFILE("SolverImplementation::CalculateForces", "", TAU_DEFAULT);

    this->currtime = currtime;
    forcecalc->calculate();
}

void UG_IF::SolverImplementation::EnforceDirichletBC(double currtime)
{
    TAU_PROFILE("SolverImplementation::EnforceDirichletBC", "", TAU_DEFAULT);
    
    if(direnforcer)
        direnforcer->EnforceValues(currtime);
}

