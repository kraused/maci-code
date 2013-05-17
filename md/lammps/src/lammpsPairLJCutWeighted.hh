
// vi: tabstop=4:expandtab

// vi: tabstop=4

#ifndef MD_LAMMPS_SRC_LAMMPS_PAIR_LJ_CUT_WEIGHTED_HH_INCLUDED
#define MD_LAMMPS_SRC_LAMMPS_PAIR_LJ_CUT_WEIGHTED_HH_INCLUDED 1

#include "lammps.h"
#include "pair.h"

/// Forward declare NeighList
namespace LAMMPS_NS {
    class NeighList;
}


namespace lammps {

    /// PairLJCutWeighted: Implements the weighted Lennard-Jones potential.
    ///
    /// In most respects this class equals PairLJCut but it does not support
    /// rESPA.
    ///
    /// FIXME: This could be a licensing issue ...
    ///
    class PairLJCutWeighted : public LAMMPS_NS::Pair
    {

    public:
        /// Create a new instance
        PairLJCutWeighted(LAMMPS_NS::LAMMPS *lmp);

        /// Destructor
        ~PairLJCutWeighted();

        /// workhorse routine that computes pairwise interactions
        void compute(int, int);

        /// reads the input script with arguments
        void settings(int, char**);

        /// set coefficients for one i,j type pair
        void coeff(int, char **);

        /// initialization specific to this pair style
        void init_style();

        /// ?
        void init_list(int, LAMMPS_NS::NeighList *);

        /// perform initialization for one i,j type pair
        double init_one(int, int);

        /// force and energy of a single pairwise interaction between 2 atoms
        double single(int, int, int, int, double, double, double, double &);

    private:
        double cut_global;
        double **cut;
        double **epsilon,**sigma;
        double **lj1,**lj2,**lj3,**lj4,**offset;

        /// Allocate all arrays and set allocated to 1.
        void allocate();
    };


    /** Create a new AtomVecMultiscaleFull
      */
    LAMMPS_NS::Pair* NewPairLJCutWeighted(LAMMPS_NS::LAMMPS *);

}

#endif


