
// vi: tabstop=4:expandtab

// vi: tabstop=4

#ifndef MD_LAMMPS_SRC_LAMMPS_COMPUTE_LIST_HH_INCLUDED
#define MD_LAMMPS_SRC_LAMMPS_COMPUTE_LIST_HH_INCLUDED 1

#include "compute.h"

namespace lammps
{

    /// Forwarding
    class Instance;

    /// ComputeList: Collection of compute instances used by the 
    ///              code to perform measurements
    class ComputeList
    {

    public:
        /// Install all computes, i.e.~register them in Lammps and
        /// set the pointers to these instances
        /// @param[in]  lmp     the Lammps instance
        void Install_computes(Instance* lmp);

        /// Compute the kinetic energy of all particles and store
        /// them in ke_per_atom->vector_atom
        void Compute_ke();

        /// Query the kinetic energy of a particle
        /// @param[in]  i   Particle id (local)
        double Query_ke(size_t i) const
        {
            return ke_per_atom->vector_atom[i];
        }

        /// Compute the potential energy of all particles and store
        /// them in pe_per_atom->vector_atom
        void Compute_pe();

        /// Query the potential energy of a particle
        /// @param[in]  i   Particle id (local)
        double Query_pe(size_t i) const
        {
            return pe_per_atom->vector_atom[i];
        }

        /// Compute the stress of all particles and store them in
        /// stress_per_atom->array_atom
        void Compute_stress();

        /// Query the stress of a particle
        /// @param[in]  i   Particle Id (local)
        /// @param[in]  j   x dimension
        /// @param[in]  k   y dimension
        double Query_stress(size_t i, int j, int k) const
        {
            /// The stress is stored in a compressed storage 
            /// using the symmetry of the stress
            int map[3][3] = { 
                    { 0, 1, 2 }, 
                    { 1, 3, 4 }, 
                    { 2, 4, 5 } };

            return stress_per_atom->array_atom[i][map[j][k]];
        }

    private:
        
        /// Compute for kinetic energy per particle
        LAMMPS_NS::Compute* ke_per_atom;

        /// Compute for potential energy per particle
        LAMMPS_NS::Compute* pe_per_atom;

        /// Compute the stress per particle
        LAMMPS_NS::Compute* stress_per_atom;
    };

}

#endif


