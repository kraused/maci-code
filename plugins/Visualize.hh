
// vi: tabstop=4:expandtab

#ifndef PLUGINS_VISUALIZE_HH_INCLUDED
#define PLUGINS_VISUALIZE_HH_INCLUDED 1

#include "Plugin.hh"
#include "ParticleList.hh"
#include "polylib/polylib.hh"

#include <string>
#include <stdio.h>

/// Forwarding
class Maci;


namespace plugins 
{

    /// Visualize: A plugin for visualizing the particle and finite element
    ///        systems
    class Visualize : public Plugin
    {

    public:
        /// Create a new energy measure plugin
        /// @param[in]  app     the Maci application instance
        /// @param[in]  themask     mask value
        /// @param[in]  theprefix   prefix for the output files
        Visualize(Maci* app, int themask, const char *theprefix);

        /// Copy constructor
        /// @param[in]  o   the other instance
        Visualize(const Visualize &o);

        /// Clone function
        Visualize* clone() const;


        /// Write the reference configuration
        void setup();

        /// Write an output file
        void post_step();


    private:
        /// The mask is used to decide at which timesteps the measurements
        /// should be done. Only timesteps which are a multiple of the mask are
        /// considered.
        int         mask;
        /// Current timestep
        long        step;   
        /// Output file prefix
        std::string prefix;

        /// Write a file
        void write(const std::string& filename);
    };

}

#endif

