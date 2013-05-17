
// vi: tabstop=4:expandtab

#ifndef PLUGINS_SHOW_PROGRESS_HH_INCLUDED
#define PLUGINS_SHOW_PROGRESS_HH_INCLUDED 1

#include "Plugin.hh"

/// Forwarding
class Maci;


namespace plugins 
{

    /// ShowProgress: A plugin for monitoring the progress of the simulation
    class ShowProgress : public Plugin
    {

    public:
        /// Create a new energy measure plugin
        /// @param[in]  app         the Maci application instance
        /// @param[in]  themask     mask value
        ShowProgress(Maci* app, int themask);

        /// Copy constructor
        /// @param[in]  o   the other instance
        ShowProgress(const ShowProgress &o);

        /// Clone function
        ShowProgress* clone() const;

        /// Announce start of time integration
        void pre_integration();

        /// Announce end of time integration
        void post_integration();
        
        /// Start time measurement for this step
        void pre_step();

        /// End timing for this step and print to the
        /// logfile if this step is divisible by themask
        void post_step();

        /// Using pre_force() and post_force() we can measure
        /// the time spend in force evaluation
        void pre_force();
        void post_force();


    private:
        /// The mask is used to decide at which timesteps the measurements
        /// should be done. Only timesteps which are a multiple of the mask are
        /// considered.
        int     mask;
        /// Current timestep
        long    step;   
        /// The sum of the time in force calculation and the full step
        double  time_force, time_step, t0, t1, t2, t3;
    };

}

#endif

