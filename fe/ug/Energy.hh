
// vi: tabstop=4:expandtab

#ifndef MACI_FE_UG_ENERGY_HH_INCLUDED
#define MACI_FE_UG_ENERGY_HH_INCLUDED 1

namespace fe
{
        namespace interface 
    {

        /// Forwarding
        class Solver;

        /// Compute the kinetic energy of the 
        /// @param  solver  the solver instance
        double KineticEnergy(Solver& solver);

        /// Compute the potential energy of the 
        /// @param  solver  the solver instance
        double PotentialEnergy(Solver& solver);

        }
}

#endif

