
// vi: tabstop=4:expandtab

#ifndef MACI_NODAL_VALUE_PACKER_HH_INCLUDED
#define MACI_NODAL_VALUE_PACKER_HH_INCLUDED 1

#include "DesignByContract.hh"
#include "fe/FiniteElements.hh"


/// Pack displacements
class NodalValuePackDispl
{
public:
    void operator()(const fe::interface::Node& nd, double *x) const
    {
        CHECK_PRECOND_DEBUG(x);

        for(int i = 0; i < DIM; ++i)
            x[i] = fe::interface::StateVariables::Displ(*gcpe()->GetFESolver(), nd, i);
    }
};

/// Pack velocities
class NodalValuePackVel
{
public:
    void operator()(const fe::interface::Node& nd, double *x) const
    {
        CHECK_PRECOND_DEBUG(x);

        for(int i = 0; i < DIM; ++i)
        x[i] = fe::interface::StateVariables::Vel(*gcpe()->GetFESolver(), nd, i);
    }
};

#endif

