
#include "Callback.hh"

void (*RattleIntegrator_CorrectAfterKickDriftCB)(RattleIntegrator* self) = 0;
void (*RattleIntegrator_CorrectAfterKickCB)(RattleIntegrator* self) = 0;

void (*TimeIntegrator_ApplyDamperCB)(TimeIntegrator* self) = 0;

void (*TimeIntegrator_plugins_setupCB)(TimeIntegrator* self) = 0;
void (*TimeIntegrator_plugins_pre_integrationCB)(TimeIntegrator* self) = 0;
void (*TimeIntegrator_plugins_post_integrationCB)(TimeIntegrator* self) = 0;
void (*TimeIntegrator_plugins_pre_forceCB)(TimeIntegrator* self) = 0;
void (*TimeIntegrator_plugins_post_forceCB)(TimeIntegrator* self) = 0;
void (*TimeIntegrator_plugins_pre_stepCB)(TimeIntegrator* self) = 0;
void (*TimeIntegrator_plugins_post_stepCB)(TimeIntegrator* self) = 0;

