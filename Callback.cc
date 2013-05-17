
// vi: tabstop=4:expandtab

#include "Callback.hh"
#include "CallbackExtern.hh"
#include "TimeIntegrator.hh"
#include "RattleIntegrator.hh"


void RattleIntegrator_CorrectAfterKickDrift(RattleIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->CorrectAfterKickDrift();
}

void RattleIntegrator_CorrectAfterKick(RattleIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->CorrectAfterKick();
}

void TimeIntegrator_ApplyDamper(TimeIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->ApplyDamper();
}

void TimeIntegrator_plugins_setup(TimeIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->plugins_setup();
}

void TimeIntegrator_plugins_pre_integration(TimeIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->plugins_pre_integration();
}

void TimeIntegrator_plugins_post_integration(TimeIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->plugins_post_integration();
}

void TimeIntegrator_plugins_pre_force(TimeIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->plugins_pre_force();
}

void TimeIntegrator_plugins_post_force(TimeIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->plugins_post_force();
}

void TimeIntegrator_plugins_pre_step(TimeIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->plugins_pre_step();
}

void TimeIntegrator_plugins_post_step(TimeIntegrator* self)
{
    CHECK_PRECOND_DEBUG(self);
    self->plugins_post_step();
}


void InstallCallbacks()
{
    RattleIntegrator_CorrectAfterKickDriftCB  = RattleIntegrator_CorrectAfterKickDrift;
    RattleIntegrator_CorrectAfterKickCB       = RattleIntegrator_CorrectAfterKick;
    
    TimeIntegrator_ApplyDamperCB              = TimeIntegrator_ApplyDamper;

    TimeIntegrator_plugins_setupCB            = TimeIntegrator_plugins_setup;
    TimeIntegrator_plugins_pre_integrationCB  = TimeIntegrator_plugins_pre_integration;
    TimeIntegrator_plugins_post_integrationCB = TimeIntegrator_plugins_post_integration;
    TimeIntegrator_plugins_pre_forceCB        = TimeIntegrator_plugins_pre_force;
    TimeIntegrator_plugins_post_forceCB       = TimeIntegrator_plugins_post_force;
    TimeIntegrator_plugins_pre_stepCB         = TimeIntegrator_plugins_pre_step;
    TimeIntegrator_plugins_post_stepCB        = TimeIntegrator_plugins_post_step;
}

