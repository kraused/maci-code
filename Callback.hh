
// vi: tabstop=4:expandtab

#ifndef MACI_CALLBACK_HH_INCLUDED
#define MACI_CALLBACK_HH_INCLUDED 1

#ifdef HAVE_CONFIG_HH
#include "config.hh"
#endif

/// Forwarding
class RattleIntegrator;
class TimeIntegrator;


/// @file Callback.hh
/// For highly modular codes like the one at hand, implementing modules in a way
/// which prevents circular references is a complicated task. For example, the
/// time integration with the LAMMPS MD code must be implemented in C++ code
/// (not on script level). Since time integration with e.g. a RATTLE integrator
/// requires correction this means that the LAMMPS code depends on the main
/// code which (obviously) depends on the LAMMPS interface itself.
///
/// To prevent these circular references (especially between DSOs) we use 
/// callbacks.
/// One should be aware of the costs of this approach and should ensure that
/// callback functions do a sufficient amount of work to amortize the costs
/// of the (indirect) function call


/// Rattle correction routines
void RattleIntegrator_CorrectAfterKickDrift(RattleIntegrator* self);
void RattleIntegrator_CorrectAfterKick(RattleIntegrator* self);

/// Modify the forces to damp high frequent terms
void TimeIntegrator_ApplyDamper(TimeIntegrator* self);

/// Apply the plugins
void TimeIntegrator_plugins_setup(TimeIntegrator* self);
void TimeIntegrator_plugins_pre_integration(TimeIntegrator* self);
void TimeIntegrator_plugins_post_integration(TimeIntegrator* self);
void TimeIntegrator_plugins_pre_force(TimeIntegrator* self);
void TimeIntegrator_plugins_post_force(TimeIntegrator* self);
void TimeIntegrator_plugins_pre_step(TimeIntegrator* self);
void TimeIntegrator_plugins_post_step(TimeIntegrator* self);


/// Install all callbacks. The functions sets the extern variable XyzCB to
/// point to the funciton Xyz
void InstallCallbacks();

#endif

