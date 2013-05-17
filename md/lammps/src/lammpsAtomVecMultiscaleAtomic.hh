
// vi: tabstop=4:expandtab

#ifndef MD_LAMMPS_SRC_LAMMPS_ATOM_VEC_MULTISCALE_HH_INCLUDED
#define MD_LAMMPS_SRC_LAMMPS_ATOM_VEC_MULTISCALE_HH_INCLUDED 1

#include "md/lammps/src/lammpsPiggyback.hh"

#include "lammps.h"
#include "atom_vec.h"

namespace lammps 
{

    /// AtomVecMultiscaleAtomic: AtomVecAtomic + piggyback'ed data
    class AtomVecMultiscaleAtomic : public LAMMPS_NS::AtomVec
    {

    public:
        /// Create a new instance
        AtomVecMultiscaleAtomic(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg);

        /// Grow atom arrays
        void grow      (int);
        /// Reset local array pointers
                void grow_reset();
        void copy      (int, int);

        int  pack_comm      (int, int *, double *, int, int *);
        int  pack_comm_vel  (int, int *, double *, int, int *);
        void unpack_comm    (int, int, double *);
        void unpack_comm_vel(int, int, double *);

        int  pack_reverse  (int, int, double *);
        void unpack_reverse(int, int *, double *);

        int  pack_border      (int, int *, double *, int, int *);
        int  pack_border_vel  (int, int *, double *, int, int *);
        void unpack_border    (int, int, double *);
        void unpack_border_vel(int, int, double *);

        int  pack_exchange  (int, double *);
        int  unpack_exchange(double *);

        int  size_restart  ();
        int  pack_restart  (int, double *);
        int  unpack_restart(double *);

        void create_atom(int, double *);
        void data_atom  (double *, int, char **);

        double memory_usage();

    public:
        int   *tag, *type, *mask, *image;
        double    **x, **v, **f, **x0;
        /// The piggyback data
        Piggyback *pb;
    };

    /** Create a new AtomVecMultiscaleAtomic
     */
    LAMMPS_NS::AtomVec* NewAtomVecMultiscaleAtomic(LAMMPS_NS::LAMMPS *, int, char **);

}

#endif


