
// vi: tabstop=4:expandtab

#ifndef MACI_TIME_INTEGRATOR_HH_INCLUDED
#define MACI_TIME_INTEGRATOR_HH_INCLUDED 1

#include "Pointers.hh"

/// Forwarding
class Maci;
class DampingMethod;
class DAEqCorrector;


/// Base class for all time integrators
class TimeIntegrator : public Pointers
{

public:
    /// Create a new instance.
    /// In the constructor, the plugins are all loaded
    TimeIntegrator(Maci* app);

    /// Call the corresponding function in the PluginTable
    void plugins_setup();
    void plugins_pre_integration();
    void plugins_post_integration();
    void plugins_pre_force();
    void plugins_post_force();
    void plugins_pre_step();
    void plugins_post_step();

    /// Apply the damper
    void ApplyDamper();
    
    /// public so that they can be accessed by
    /// the plugins but we don't want SWIG to create
    /// wrappers for them
#ifdef SWIG
private:
#endif

    /// Damper method used for removing e.g., high fluctuations
    /// from the MM displacement field in the handshake region
    DampingMethod*  damper;

    /// Corrector for the integration of differential algebraic
    /// equations. This pointer is only != NULL if the derived
    /// class uses a corrector
    DAEqCorrector*  corrector;
};

#endif

