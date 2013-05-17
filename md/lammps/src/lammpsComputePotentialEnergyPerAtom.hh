
// vi: tabstop=4:expandtab

// vi: tabstop=4

#ifndef MD_LAMMPS_SR_LAMMPS_COMPUTE_POTENTIAL_ENERGY_HH_INCLUDED
#define MD_LAMMPS_SR_LAMMPS_COMPUTE_POTENTIAL_ENERGY_HH_INCLUDED

#include "compute.h"

namespace lammps
{

    /// Forwarding Instance;
    class Instance;


    /// ComputePotentialEnergyPerAtom: Compute the potential energy of each
    /// atom
    ///
    /// Based on the ComputePEAtom class in Lammps. Supports only pair
    /// pontials.
    ///
    class ComputePotentialEnergyPerAtom : public LAMMPS_NS::Compute
    {

    public:
        /// Create a new instance
        ComputePotentialEnergyPerAtom(Instance* lmp);

        /// Destructor
        ~ComputePotentialEnergyPerAtom();

        /// Unused
        void init() { }

        /// Compute for each atom
        void compute_peratom();

        /// Communication routines
        int    pack_reverse_comm(int, int  , double *);
        void unpack_reverse_comm(int, int *, double *);

    private:
        int     nmax;
        double* energy;

        /// Clear the forces. This is necessary because the
        /// potential energy is calculated as part of the force
        /// computation and we don't want to add up the two
        /// forces
        void force_clear();

    };

}

#endif


