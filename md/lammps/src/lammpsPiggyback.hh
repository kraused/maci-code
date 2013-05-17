
// vi: tabstop=4:expandtab

#ifndef MD_LAMMPS_SRC_LAMMPS_PIGGYBACK_HH_INCLUDED
#define MD_LAMMPS_SRC_LAMMPS_PIGGYBACK_HH_INCLUDED 1

#include <algorithm>

namespace lammps {

    /// Piggyback: Data to be "piggybacked" on each particle
    /// The piggyback data type allows the implementation to
    /// add additional information to each particle. The implementation
    /// of the MD toolkit guarantees that the piggyback data is
    /// always available for local particles, i.e., it is transmitted
    /// together with the particle if the owner is changed.
    struct Piggyback
    {

        /// Maximal number of target FE processors for one particle
        static const int MAX_FEPROCS = 8;
        /// Maximal number of target BD processors for one particle
        static const int MAX_BDPROCS = 8;
        /// The maximum of MAX_FEPROCS and MAX_BDPROCS. If additional
        /// targets and displacements (because of an additional
        /// communication step) is added this number must be adopted
        static const int MAX_TARGETS = 8;

        int     source;
        int     bdIndex;
        int     feprocs[MAX_FEPROCS];
        int     fedispl[MAX_FEPROCS];
        int     origin;
        int     locIndex;
        int     bdprocs[MAX_BDPROCS];
        int     bddispl[MAX_BDPROCS];

        /// The weighting coefficient ($\alpha$ or $w$) which is used
        /// to prevent double counting of the energy in the overlapping
        /// domain decomposition
        double  weight;
        /// The damping coefficient for the PML type damping
        double  dampingD;

        /// Get number of members
        static inline int numMembers()
        {
            return 6 + 2*MAX_FEPROCS + 2*MAX_BDPROCS;
        }

    };

    /// Serialize a piggyback structure
    /// @param[in]  pb  piggyback instance
    /// @param[in]  buf the buffer space
    /// @returns    the number of doubles written
    int packPiggyback(Piggyback *pb, double *buf);

    /// Deserialize a piggyback structure
    /// @param[in]  buf the buffer space
    /// @param[in]  pb  piggyback instance
    /// @returns    the number of doubles read
    int unpackPiggyback(double *buf, Piggyback *pb);

}

#endif

