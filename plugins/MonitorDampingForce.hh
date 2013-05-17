
// vi: tabstop=4:expandtab

#ifndef PLUGINS_MONITOR_DAMPING_FORCE_HH_INCLUDED
#define PLUGINS_MONITOR_DAMPING_FORCE_HH_INCLUDED 1

#include "Plugin.hh"
#include "ParticleList.hh"
#include "polylib/polylib.hh"

#include <string>
#include <stdio.h>

/// Forwarding
class Maci;


namespace plugins 
{

    /// MonitorDampingForce: A plugin for monitoring the forces added to the
    ///          force field for damping
    class MonitorDampingForce : public Plugin
    {

    public:
        /// Create a new energy measure plugin
        /// @param[in]  app the Maci application instance
        /// @param[in]  outfile name for the output files or "-"
        ///         if stdout should be used
        MonitorDampingForce(Maci* app, const char *outfile);

        /// Copy constructor
        /// @param[in]  o   the other instance
        MonitorDampingForce(const MonitorDampingForce &o);

        /// Clone function
        MonitorDampingForce* clone() const;

        /// Opens the output file
        void pre_integration();

        /// Closes the output file
        void post_integration();

        /// Write to output file
        void post_step();


    private:
        /// Output file outfile
        std::string outfile;
        /// Current timestep
        long        step;
        /// The output file
        FILE*       fo;
    
    };

}

#endif

