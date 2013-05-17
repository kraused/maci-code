
// vi: tabstop=4:expandtab

// vi: tabstop=4

#ifndef MD_LAMMPS_SR_LAMMPS_STRESS_ENERGY_HH_INCLUDED
#define MD_LAMMPS_SR_LAMMPS_STRESS_ENERGY_HH_INCLUDED

#include "compute.h"

namespace lammps
{

    /// Forwarding Instance;
    class Instance;


    /// ComputeStressPerAtom: Compute the stress of each atom
    ///
    /// Based on the ComputeStressAtom class in Lammps but usesed
    /// the correct stress formula
    ///
    class ComputeStressPerAtom : public LAMMPS_NS::Compute
    {

    public:
        /// Create a new instance
        ComputeStressPerAtom(Instance* lmp);

        /// Destructor
        ~ComputeStressPerAtom();

        /// Unused
        void init() { }

        /// Compute for each atom
        void compute_peratom();

        /// Communication routines
        int    pack_reverse_comm(int, int  , double *);
        void unpack_reverse_comm(int, int *, double *);

    private:
        int      nmax;
        double** stress;
    };

}

#endif


