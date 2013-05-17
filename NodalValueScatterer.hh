
// vi: tabstop=4:expandtab

#ifndef MACI_NODAL_VALUE_SCATTERED_HH_INCLUDED
#define MACI_NODAL_VALUE_SCATTERED_HH_INCLUDED 1

#include "DesignByContract.hh"

#include "fe/FiniteElements.hh"


/// Assign velocities
class NodalValueScatterVel
{
public:
    void operator()(fe::interface::Node& nd, double* x) const
    {
        CHECK_PRECOND_DEBUG(x);

        for(int i = 0; i < DIM; ++i)
            fe::interface::StateVariables::Vel(*gcpe()->GetFESolver(), nd, i) = x[i];
    }
};

#endif

