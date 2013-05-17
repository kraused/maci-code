
// vi: tabstop=4:expandtab

#ifndef MD_TREMOLO_SRC_TREMOLO_INSTANCE_HH_INCLUDED
#define MD_TREMOLO_SRC_TREMOLO_INSTANCE_HH_INCLUDED 1

#include <mpi.h>

#include "treInstance.hh"

namespace tremolo {

    /// Instance: Central tremolo class
    /// An instance of the Tremolo software running on a dedicated set of
    /// processes. This class gives access to particle data,
    /// time integration, external forces and load balancing.
    class Instance : public instance_t
    {
        typedef instance_t Base;

    public:
        /// Create a new instance
        /// @param[in]  comm    MPI Communicator
        Instance(MPI_Comm comm);

        /// Destructor
        ~Instance();

        /// Set processors
        /// @param[in]  processors  the splitting of the communicator
        ///             into a cartesian geometry
        void SetProcessors(const int processors[3]);

        /// Set computing box. The tremolo interface only supports
        /// rectangular boxes with left corner (0.0, 0.0, 0.0)
        /// @param[in]  box the left-lower and right-upper corner
        ///         of the three dimensional simulation
        ///         box. The boundary conditions are
        ///         periodic
        void SetSimulationBox(const double box[2][3]);

        /// Set the number of external forces
        /// @param[in]  n   number of external forces used
        /// @param[in]  F   the external forces per exttype.
        ///         
        void SetExtforces(int n, const double F[][3]);

        /// Tell Tremolo to use the standard Lennard-Jones potential
        /// @param[in]  cutoff  The potential cutoff. This value is used for
        ///         the cutoff of potential calculations but also
        ///         as the cell rcut
        /// @param[in]  eps Energy scale
        /// @param[in]  sigma   Length scale
        void Set_Pair_Potential_LJ(double cutoff, 
                       double eps, 
                       double sigma)
        {
            cellrcut = cutoff;
            rcut     = cutoff;

            ljeps   = eps;
            ljsigma = sigma;
        }

        /// Tell Tremolo to use the weighted Lennard-Jones potential. This
        /// triggers also the use of weighted masses in time integration
            /// @param[in]  cutoff  The potential cutoff. This value is used for
            ///              the cutoff of potential calculations but also
            ///              as the cell rcut
            /// @param[in]  eps     Energy scale
            /// @param[in]  sigma   Length scale
        void Set_Pair_Potential_LJ_Weighted(double cutoff, 
                            double eps, 
                            double sigma)
        {
            Set_Pair_Potential_LJ(cutoff, eps, sigma);
            Enable_wgt();
        }

        /// Compute the center of mass of the particle system. The
        /// function is collective, i.e. it must be called by all MD
        /// processors to make progress
        /// @param[out] X   center of mass of the particle system
        void CenterOfMass(double X[3]) const;

        /// Initialization function. The function reads atom data from
        /// the specified file
        /// @param[in]  filename    the atom data file
        void Init(const char *filename);

        /// Access Types
        /// TODO These are duplicated in the interface and need
        ///      to be kept in sync. Maybe we find a better solution
        enum 
        {
            INVALID     = 0x000,
            // Read/Write access to positions
            RD_POS      = 0x001,
            WR_POS      = 0x002,
            // Read/Write access to velocities
            RD_VEL      = 0x004,
            WR_VEL      = 0x008,
            // Read/Write access to reference positions
            RD_REFP     = 0x010,
            WR_REFP     = 0x020,
            // Read/Write access to forces
            RD_FORCE    = 0x040,
            WR_FORCE    = 0x080,
            // Read/Write access to masses
            RD_MASS     = 0x100,
            WR_MASS     = 0x200,
            // Read/Write access to piggyback data
            RD_PIGGYBACK    = 0x400,
            WR_PIGGYBACK    = 0x800
        };
        
        /// Start an access epoch
        void AccessBegin(int type);
        
        /// End an access epoch
        void AccessEnd();
        
        /// Returns true if the access to particle data is legal
        inline bool CanAccess() const
        {
            return in_epoch;
        }

        /// The function Wgt_is_Enabled returns true if
        /// weigthing must be taken into account in the time
        /// integration etc.
        inline bool Wgt_enabled() const
        {
            return wgt_enabled;
        }

        /// Compute the number of local atoms
        long NlocalAtoms() const;

    private:
        /// Right corner of the simulation box
        double      corner[3];
        /// Number of external forces
        int         nx;
        /// External forces
        double      (*fx)[3];
        /// Cell rcut
        double      cellrcut;
        /// rcut for the short-ranged force
        double      rcut;
        /// Lennard jones energy scale
        double      ljeps;
        /// Lennard jones lengthscale
        double      ljsigma;

        // True if an access epoch is started
        bool       in_epoch;
        // The type of the epoch
        int    epoch_type;

        /// If wgt_enabled is true, we use a weighted MD potential and hence
        /// we also need to use weighting in the time integration, calculation of
        /// kinetic energy ...
        bool wgt_enabled;

        /// Set wgt_enable to true and trigger internal changes (to global
        /// variables that are necessary to tell Tremolo what to do)
        void Enable_wgt();

    };

}

#endif

