
// vi: tabstop=4:expandtab

// vi: tabstop=4

#include "lammpsComputePotentialEnergyPerAtom.hh"
#include "lammpsInstance.hh"

#include "memory.h"
#include "force.h"
#include "pair.h"
#include "comm.h"
#include "neighbor.h"

using namespace lammps;


static int       nargs  = 3;
static const char* args[] = { "11", "all", "pe/atom", NULL };

ComputePotentialEnergyPerAtom::ComputePotentialEnergyPerAtom(Instance* lmp)
: Compute(lmp, nargs, (char** )args), nmax(0), energy(NULL)
{
    peratom_flag = 1;
}

ComputePotentialEnergyPerAtom::~ComputePotentialEnergyPerAtom()
{
        memory->sfree(energy);
}

void ComputePotentialEnergyPerAtom::compute_peratom()
{
    if(atom->nmax > nmax) {
        memory->sfree(energy);
        nmax        = atom->nmax;
        energy    = (double *)memory->smalloc(nmax*sizeof(double),"pe/atom:energy");
        vector_atom = energy;
    }

    int nlocal = atom->nlocal;
    int npair  = nlocal;
    int ntotal = nlocal;

    if(force->newton) {
        npair  += atom->nghost;
        ntotal += atom->nghost;
    }

//  force_clear();

    if(force->pair) {
//      /// Trigger a force evaluation with per-atom energy calculation (2)
//      /// and no virial computation (0).
//      force->pair->compute(2, 0);

//      for(i = 0; i < ntotal; ++i)
//          energy[i] = force->pair->eatom[i];
    }

    /// TODO We could easily do the same for 
    ///      bonds and friends

    if(force->newton) {
        comm->reverse_comm_compute(this);
    }

    /// We do not deal with groups here ...
}

int ComputePotentialEnergyPerAtom::pack_reverse_comm(int n, int first, double *buf)
{
    int i, k = 0, last = first + n;

    for(i = first; i < last; ++i)
        buf[k++] = energy[i];

    return 1;
}

void ComputePotentialEnergyPerAtom::unpack_reverse_comm(int n, int *list, double *buf)
{
    int i, k = 0;

    for(i = 0; i < n; ++i)
        energy[list[i]] += buf[k++];
}

/// Helper function used in force_clear() to set the forces for
/// a set of particles (with contiguous local indices) to zero
static inline void fill_zeroes(double** f, int istart, int iend)
{
    for(; istart != iend; ++istart)
        std::fill(f[istart], f[istart]+3, 0.0);
}

/// Code based on Verlet::force_clear()
void ComputePotentialEnergyPerAtom::force_clear()
{ 
    if(neighbor->includegroup == 0)
    {
        int nall = (force->newton) ? (atom->nlocal + atom->nghost) : atom->nlocal;

        fill_zeroes(atom->f, 0, nall);
        /// TODO Clear atom->torque?
    }
    else
    {
        fill_zeroes(atom->f, 0, atom->nfirst);
        /// TODO Clear atom->torque?

        if(force->newton)
        {
            fill_zeroes(atom->f, atom->nlocal, atom->nlocal + atom->nghost);
            /// TODO Clear atom->torque?
        }
    }
}

