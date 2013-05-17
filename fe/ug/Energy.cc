
// vi: tabstop=4:expandtab

#include "Energy.hh"
#include "Solver.hh"


double fe::interface::KineticEnergy(Solver& solver)
{
    return solver.GetImplementation()->KineticEnergy();
}

double fe::interface::PotentialEnergy(Solver& solver)
{
    return solver.GetImplementation()->PotentialEnergy();
}

