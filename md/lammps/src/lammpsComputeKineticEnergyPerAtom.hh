
// vi: tabstop=4:expandtab

// vi: tabstop=4

#ifndef MD_LAMMPS_SR_LAMMPS_COMPUTE_KINETIC_ENERGY_HH_INCLUDED
#define MD_LAMMPS_SR_LAMMPS_COMPUTE_KINETIC_ENERGY_HH_INCLUDED

#include "compute.h"

namespace lammps
{

    /// Forwarding Instance;
    class Instance;


    /// ComputeKineticEnergyPerAtom: Compute the kinetic energy of each
    /// atom
    ///
    /// Based on the ComputeKEAtom class in Lammps.
    ///
    class ComputeKineticEnergyPerAtom : public LAMMPS_NS::Compute
    {

    public:
        /// Create a new instance
        ComputeKineticEnergyPerAtom(Instance* lmp);

        /// Destructor
        ~ComputeKineticEnergyPerAtom();

        /// Unused
        void init() { }

        /// Compute for each atom
        void compute_peratom();

    private:
        int     nmax;
        double* energy;

    };

}

#endif


