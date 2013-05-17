
// vi: tabstop=4:expandtab

#ifndef MACI_PLUGIN_HH_INCLUDED
#define MACI_PLUGIN_HH_INCLUDED 1

#include <string>

#include "Pointers.hh"


/// Plugin: Base class for all plugin extensions to the code
/// Plugins can be used to measure quantities or modify the system after each
/// timestep
///
/// Plugins are setup after the individual scales have been initialized
/// properly but before initial values are applied. No guarantee is made
/// for the time of construction. Hence, the constructor should not do much
/// more than inializing (scalar) member variables. Everything else should
/// be done in the setup call.
///
/// Each plugin must define a copy constructor as we store them
/// in an array.
class Plugin : public Pointers
{

public:
    
    /// Create a new named plugin. 
    /// @param[in]  app the Maci application instance
    /// @param[in]  name    name of the plugin
    Plugin(Maci* app, const std::string& name);

    /// Each member must implement a clone function
    virtual Plugin* clone() const = 0;

    /// Set up the plugin. This function is called before initial
    /// conditions are applied allowing e.g.~to record the ground
    /// energy level of the atomistic lattice.
    virtual void setup() { }

    /// Function called before the time integration starts. This
    /// means that initial conditions have already been applied.
    virtual void pre_integration() { }

    /// Function called after the time integration stopped.
    virtual void post_integration() { }

    /// Function called before force evaluation.
    virtual void pre_force() { }

    /// Function called after force evaluation.
    virtual void post_force() { }

    /// Function called before each timestep. Note that pre_force()/post_force()
    /// can be called before the start of the time loop and hence before
    /// this function (in order words, it should not be assumed that pre_step()
    /// is caled before pre_force(), ...).
    virtual void pre_step() { }

    /// Function called after each timestep
    virtual void post_step() { }

    /// The destructor. Basically here to make the compiler happy.
    virtual ~Plugin() {}

};

#endif

