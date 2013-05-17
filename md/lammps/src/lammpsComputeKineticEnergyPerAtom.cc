
// vi: tabstop=4:expandtab

// vi: tabstop=4

#include "lammpsComputeKineticEnergyPerAtom.hh"
#include "lammpsInstance.hh"

#include "memory.h"
#include "force.h"
#include "pair.h"
#include "comm.h"

using namespace lammps;


static int         nargs  = 3;
static const char* args[] = { "12", "all", "ke/atom", NULL };

ComputeKineticEnergyPerAtom::ComputeKineticEnergyPerAtom(Instance* lmp)
: Compute(lmp, nargs, (char** )args), nmax(0), energy(NULL)
{
    peratom_flag = 1;
}

ComputeKineticEnergyPerAtom::~ComputeKineticEnergyPerAtom()
{
        memory->sfree(energy);
}

/// Short cut notation for getting the weighting of an
/// atom
#define WGT(i)  ((Piggyback* )atom->pb)[i].weight

void ComputeKineticEnergyPerAtom::compute_peratom()
{
    int i;

    if(atom->nmax > nmax) {
        memory->sfree(energy);
        nmax        = atom->nmax;
        energy      = (double *)memory->smalloc(nmax*sizeof(double),"ke/atom:energy");
        vector_atom = energy;
    }

    int    nlocal = atom->nlocal;
    double **v    = atom->v;
    double *mass  = atom->mass;
    double *rmass = atom->rmass;
    int    *mask  = atom->mask;
    int    *type  = atom->type;

    /// Not very nice but since the lmp variable comes from Lammps there is
    /// no other way than this or modifying the Lammps code.
    bool   wgt_enabled = static_cast<lammps::Instance*>(lmp)->Wgt_enabled();

    if(rmass) {
        if(wgt_enabled) {
            for(i = 0; i < nlocal; ++i)
                energy[i] = (mask[i] & groupbit)* 0.5*rmass[i]*(v[i][0]*v[i][0] + v[i][1]*v[i][1] + v[i][2]*v[i][2])*WGT(i);
        } else {
            for(i = 0; i < nlocal; ++i)
                energy[i] = (mask[i] & groupbit)* 0.5*rmass[i]*(v[i][0]*v[i][0] + v[i][1]*v[i][1] + v[i][2]*v[i][2]);
        }
    } else {
        if(wgt_enabled) {
            for(i = 0; i < nlocal; ++i)
                energy[i] = (mask[i] & groupbit)* 0.5*mass[type[i]]*(v[i][0]*v[i][0] + v[i][1]*v[i][1] + v[i][2]*v[i][2])*WGT(i);
        } else {
            for(i = 0; i < nlocal; ++i)
                energy[i] = (mask[i] & groupbit)* 0.5*mass[type[i]]*(v[i][0]*v[i][0] + v[i][1]*v[i][1] + v[i][2]*v[i][2]);
        }
    }
}

