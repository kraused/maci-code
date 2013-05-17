
// vi: tabstop=4:expandtab

// vi: tabstop=4

#include "lammpsComputeStressPerAtom.hh"
#include "lammpsInstance.hh"

#include "memory.h"
#include "force.h"
#include "pair.h"
#include "comm.h"

using namespace lammps;


static int         nargs  = 3;
static const char* args[] = { "13", "all", "stress/atom", NULL };

ComputeStressPerAtom::ComputeStressPerAtom(Instance* lmp)
: Compute(lmp, nargs, (char** )args), nmax(0), stress(NULL)
{
    peratom_flag = 1;
}

ComputeStressPerAtom::~ComputeStressPerAtom()
{
        memory->sfree(stress);
}

void ComputeStressPerAtom::compute_peratom()
{
    int i, j;

    if (atom->nmax > nmax) {
        memory->destroy_2d_double_array(stress);
        nmax       = atom->nmax;
        stress     = memory->create_2d_double_array(nmax,6,"stress/atom:stress");
        array_atom = stress;
    }

    int nlocal = atom->nlocal;
    int npair  = nlocal;
    int ntotal = nlocal;

    if(force->newton) {
        npair  += atom->nghost;
        ntotal += atom->nghost;
    }

    if(force->pair) {
        /// Trigger a force evaluation with no energy calculation (0)
        /// and virial computation per-atom (4)
        force->pair->compute(0, 4);

        for(i = 0; i < npair; ++i)
            for(j = 0; j < 6; ++j)
                stress[i][j] = force->pair->vatom[i][j];
    }

    /// TODO We could easily do the same for 
    ///      bonds and friends

    if(force->newton) {
        comm->reverse_comm_compute(this);
    }

    /// We do not deal with groups here ...
}

int ComputeStressPerAtom::pack_reverse_comm(int n, int first, double *buf)
{
    int i, k = 0, last = first + n;

    for(i = first; i < last; ++i) {
        buf[k++] = stress[i][0];
        buf[k++] = stress[i][1];
        buf[k++] = stress[i][2];
        buf[k++] = stress[i][3];
        buf[k++] = stress[i][4];
        buf[k++] = stress[i][5];
    }

    return 1;
}

void ComputeStressPerAtom::unpack_reverse_comm(int n, int *list, double *buf)
{
    int i, k = 0;

    for(i = 0; i < n; ++i) {
        stress[list[i]][0] += buf[k++];
        stress[list[i]][1] += buf[k++];
        stress[list[i]][2] += buf[k++];
        stress[list[i]][3] += buf[k++];
        stress[list[i]][4] += buf[k++];
        stress[list[i]][5] += buf[k++];
    }
}

