
// vi: tabstop=4:expandtab

#ifndef MD_LAMMPS_SRC_LAMMPS_INSTANCE_HH_INCLUDED
#define MD_LAMMPS_SRC_LAMMPS_INSTANCE_HH_INCLUDED 1

#include <mpi.h>
#include <stdarg.h>

#include "md/lammps/src/lammpsPiggyback.hh"
#include "md/lammps/src/lammpsComputeList.hh"

#include "lammps.h"
#include "atom.h"

/// Forwarding
class TimeIntegrator;


namespace lammps {

    /// Instance: Central lammps class
    class Instance : public LAMMPS_NS::LAMMPS
    {

    public:
        /// Create a new Instance
        /// @param[in]  comm    the communicator
        Instance(MPI_Comm comm);

        /// Execute a command line in the LAMMPS script language
        /// The function internally appends a new line
        ///
        /// @param[in]  fmt the printf format
        void execute(const char *fmt, ...);

        /// std::string version of execute
        /// @see execute
        /// @param[in]  fmt the printf format
        void execute(const std::string &fmt, ...);

        /// Set the atom style
        /// The function takes the same argument as the atom_style
        /// LAMMPS command but ensures that piggybacked data is used
        /// @param[in]  style   the atom style (e.g. atomic or full)
        void setAtomStyle(const char *style);

        /// Register the normal Lennard-Jones potential as the pair style
        /// @param[in]  rcut    the cut-off radius
        void Set_Pair_Style_LJ(double rcut);

        /// Register the weighted Lennard-Jones potential as the
        /// pair potential to be used in the simulation. This function
        /// has a couple of side effects:
        /// It triggers the use of weighted masses in the time integration
        /// and the calculation of kinetic energies
        /// Note that the weighting function (in the PiggyBack array)
        /// must be set by the user prior to start time integration.
        ///
        /// @param[in]  rcut    the cut-off radius
        void Set_Pair_Style_LJ_Weighted(double rcut);

        /// Create a cartesian communicator from the world communicator
        /// of LAMMPS
        /// @warning The function set_procs must be called before this
        ///          function. The function can be used safely after
        ///      a file has been read or the box has been defined.
        MPI_Comm createCartWorld();

        /// Return the spatial region of the calling process if it is
        /// a box. In this case a positive value is returned. Otherwise
        /// zero is returned.
        int getSpatialRegionIfBox(double *lower, double *upper) const;

#define setattr(Type, name)                                 \
        Type name(size_t i) const                           \
        {                                                   \
            return this->atom->name[this->atom->type[i]];   \
        }                                                   \
        Type name(size_t i)                                 \
        {                                                   \
            return this->atom->name[this->atom->type[i]];   \
        }           

        setattr(double  , mass)

#undef  setattr
#define setattr(Type, name)                     \
        const Type name(size_t i) const         \
        {                                       \
            return this->atom->name[i];         \
        }                                       \
              Type name(size_t i)               \
        {                                       \
            return this->atom->name[i];         \
        }           

        setattr(double *, x)
        setattr(double *, v)
        setattr(double *, f)
        setattr(double *, x0)
        
#undef  setattr
#define setattr(Type, name)                     \
        const Type name(size_t i) const         \
        {                                       \
            return (Type )this->atom->name+i;   \
        }                                       \
              Type name(size_t i)               \
        {                                       \
            return (Type )this->atom->name+i;   \
        }   

        setattr(lammps::Piggyback *,pb)

#undef  setattr

        /// Read a data file
        /// @param[in]  filename    name of the input file
        void read(const char *filename);

        /// Get number of particles (local)
        inline size_t numParticles() const
        {
            return this->atom->nlocal;
        }

        /// Install a Verlet time integrator
        /// @param[in]  base    time integrator base function
        void installVerletIntegrator(::TimeIntegrator *base);
        
        /// Install a Rattle time integrator
        /// @param[in]  base        time integrator base function
        void installRattleIntegrator(::TimeIntegrator *base);

        /// Run the NoOpIntegrator
        void run_no_op();

        /// Execute the time integrator
        /// @param[in]  n   number of steps to execute
        void run(int n);

        /// Compute the center of mass of the atomistic system
        /// @param[out] x   center of mass
        void centerOfMass(double x[]) const;

        /// Create an initial wave
        /// @param[in]  midpt   midpoint
        void createInitialWave(const double midpt[]);

        /// Get a reference to the compute_list member
        inline lammps::ComputeList& computes()
        {
            return compute_list;
        }

        /// The function Wgt_is_Enabled returns true if
        /// weigthing must be taken into account in the time
        /// integration etc.
        inline bool Wgt_enabled() const
        {
            return wgt_enabled;
        }

        /// Start an access epoch
        void AccessBegin(int type);

        /// End an access epoch
        void AccessEnd();

        /// Returns true if the access to particle data is legal
        inline bool CanAccess() const
        {
            return in_epoch;
        }

    private:
        /// List of computes
        ComputeList compute_list;

        /// If wgt_enabled is true, we use a weighted MD potential and hence
        /// we also need to use weighting in the time integration, calculation of
        /// kinetic energy ...
        bool wgt_enabled;

        /// True if an access epoch is started
        bool in_epoch;
        /// The type of the epoch
        int  epoch_type;

        /// Access Types
        /// TODO These are duplicated in the interface and need
        ///  to be kept in synch. Maybe we find a better solution
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

    };

}

#endif


