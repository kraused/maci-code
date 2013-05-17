
// vi: tabstop=4:expandtab

#ifndef MACI_CALLBACK_EXTERN_HH_INCLUDED
#define MACI_CALLBACK_EXTERN_HH_INCLUDED 1

/// Forwarding
class RattleIntegrator;
class TimeIntegrator;


/// @file CallbackExtern.hh
/// Extern declared callback function pointer. This file can be included
/// in the md/XYZ and fe/XYZ files.

extern void (*RattleIntegrator_CorrectAfterKickDriftCB)(RattleIntegrator* self);
extern void (*RattleIntegrator_CorrectAfterKickCB)(RattleIntegrator* self);

extern void (*TimeIntegrator_ApplyDamperCB)(TimeIntegrator* self);

extern void (*TimeIntegrator_plugins_setupCB)(TimeIntegrator* self);
extern void (*TimeIntegrator_plugins_pre_integrationCB)(TimeIntegrator* self);
extern void (*TimeIntegrator_plugins_post_integrationCB)(TimeIntegrator* self);
extern void (*TimeIntegrator_plugins_pre_forceCB)(TimeIntegrator* self);
extern void (*TimeIntegrator_plugins_post_forceCB)(TimeIntegrator* self);
extern void (*TimeIntegrator_plugins_pre_stepCB)(TimeIntegrator* self);
extern void (*TimeIntegrator_plugins_post_stepCB)(TimeIntegrator* self);

#endif

