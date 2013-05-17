
// vi: tabstop=4:expandtab

#ifdef  MACI_RATTLE_CORRECTOR_HELPER_HH_INCLUDED
#error "RattleCorrectorHelper.hh included twice!"
#endif
#define MACI_RATTLE_CORRECTOR_HELPER_HH_INCLUDED


/// RattleNodalValueCorrector: Base class for all corrector-scatterer for nodes
class RattleNodalValueCorrector
{
protected:
    typedef fe::interface::Solver Solver;   
    typedef fe::interface::NodalVector<double, 3> NodalVector;
    typedef fe::interface::Node Node;

public:
    /// Create a new instance
    /// @param[in]  tstep   the timestep size
    RattleNodalValueCorrector(double tstep)
    : tau(tstep), solver(gcpe()->GetFESolver())
    {
        CHECK_PRECOND_ALWAYS(tau >= 0.);
    }

protected:
    /// The timestep size
    double tau;
    /// Pointer to the finite element solver instance
    Solver* solver;

    /// Get the velocity at a node
    /// @param[in]  nd  node
    /// @param[in]  i   direction
    double& v(Node& nd, int i) const
    {
        return fe::interface::StateVariables::Vel(*solver, nd, i);
    }

    /// Get the displacement at a node
    /// @param[in]  nd      node
    /// @param[in]  i       direction
    double& u(Node& nd, int i) const
    {
        return fe::interface::StateVariables::Displ(*solver, nd, i);
    }
};


/// RattleNodalValueCorrectorDisplVel: Corrector for nodal displacement and velocity values
class RattleNodalValueCorrectorDisplVel : public RattleNodalValueCorrector
{
public:
    RattleNodalValueCorrectorDisplVel(double tau)
    : RattleNodalValueCorrector(tau)
    {
    }

    /// Correct values at node
    /// @param nd   the node
    /// @param x    lagrange forces
    void operator()(Node& nd, const double *x) const
    {
        for(int i = 0; i < DIM; ++i) 
        {
            u(nd,i) -= x[i];
            v(nd,i) -= x[i]/tau;
        }
    }
};


/// RattleNodalValueCorrectorVel: Corrector for nodal velocity values
class RattleNodalValueCorrectorVel : public RattleNodalValueCorrector
{
public:
    RattleNodalValueCorrectorVel(double tau)
    : RattleNodalValueCorrector(tau)
    {
    }

    /// Correct values at node
    /// @param nd   the node
    /// @param x    lagrange forces
    void operator()(Node& nd, const double *x) const
    {
        for(int i = 0; i < DIM; ++i)
            v(nd,i) -= x[i];
    }
};

