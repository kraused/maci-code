
// vi: tabstop=4:expandtab

#ifndef PLUGINS_MONITOR_RESIDUAL_HH_INCLUDED
#define PLUGINS_MONITOR_RESIDUAL_HH_INCLUDED 1

#include "ResidualNormComputer.hh"
#include "Plugin.hh"
#include "ParticleList.hh"
#include "polylib/polylib.hh"

#include <string>
#include <stdio.h>

/// Forwarding
class Maci;


namespace plugins 
{

    /// MonitorResidual: A plugin for monitoring the residual g
    class MonitorResidual : public Plugin
    {

    public:
        /// Create a new energy measure plugin
        /// @param[in]  app the Maci application instance
        /// @param[in]  outfile name for the output files or "-"
        ///         if stdout should be used
        MonitorResidual(Maci* app, const char *outfile);

        /// Copy constructor
        /// @param[in]  o   the other instance
        MonitorResidual(const MonitorResidual &o);

        /// Clone function
        MonitorResidual* clone() const;

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

        /// The residual computer
        ResidualNormComputer* comp;

    };

}

#endif

