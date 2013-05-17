
// vi: tabstop=4:expandtab

#ifndef MACI_RATTLE_INTEGRATOR_HH_INCLUDED
#define MACI_RATTLE_INTEGRATOR_HH_INCLUDED 1

#include "TimeIntegrator.hh"
#include "NodalValueBuffer.hh"

/// Forwarding
class Maci;
class RattleCorrector;
class DampingMethod;


/// RattleIntegrator: Implements the RATTLE integration method
/// RATTLE is a method for integrating constrained Hamiltonian systems. It
/// is a variant of the SHAKE algorithm. In each RATTLE time step two linear
/// systems need to be solve: One system to enforce the primary 
/// (displacement) constraints and one system to enforce the secondary (velocity) 
/// constraints.
class RattleIntegrator : public TimeIntegrator
{

public:
    /// Create a new intestance
    /// @param[in]  app     the Maci application instance
    /// @param[in]  damper      the damping method
    /// @param[in]  corrector   the corrector instance used for enforcing
    ///             constraints
    RattleIntegrator(Maci* app, DampingMethod* damper, RattleCorrector* corrector);

    /// Destructor
    ~RattleIntegrator();

    /// Dispatches to the CorrectAfterKickDrift of the corrector
    void CorrectAfterKickDrift();

    /// Dispatches to the CorrectAfterKick of the corrector
    void CorrectAfterKick();

private:
    /// The Lagrange forces computed from the enforcement of the
    /// primary (displacement/position) constraints.
    /// The pointer is NULL on particle processing elements.
    NodalValueBuffer<double>* lambda; 

    /// The Lagrange forces computed from the enforcement of the
    /// secondary (velocity) constraints.
    /// The pointer is NULL on particle processing elements.
    NodalValueBuffer<double>* mu;

};

#endif

