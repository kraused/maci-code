
// vi: tabstop=4:expandtab

#include "lammpsComputeList.hh"
#include "lammpsInstance.hh"
#include "lammpsComputePotentialEnergyPerAtom.hh"
#include "lammpsComputeKineticEnergyPerAtom.hh"
#include "lammpsComputeStressPerAtom.hh"

#include <mpi.h>
#include <stdio.h>

#include "modify.h"

using namespace lammps;


void ComputeList::Install_computes(lammps::Instance* lmp)
{
    pe_per_atom = new ComputePotentialEnergyPerAtom(lmp);
    ke_per_atom = new ComputeKineticEnergyPerAtom(lmp);
    stress_per_atom = new ComputeStressPerAtom(lmp);
}

void ComputeList::Compute_ke()
{
    ke_per_atom->compute_peratom();
}

void ComputeList::Compute_pe()
{
    pe_per_atom->compute_peratom();
}

void ComputeList::Compute_stress()
{
    stress_per_atom->compute_peratom();
}

