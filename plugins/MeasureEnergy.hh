
// vi: tabstop=4:expandtab

#ifndef MACI_PLUGINS_MEASURE_ENERGY_HH_INCLUDED
#define MACI_PLUGINS_MEASURE_ENERGY_HH_INCLUDED 1

#include "Plugin.hh"
#include "ParticleList.hh"
#include "polylib/polylib.hh"

#include <string>
#include <stdio.h>

namespace plugins 
{

    /// MeasureEnergy: A plugin for measuring energy of particle and finite 
    ///            element systems
    class MeasureEnergy : public Plugin
    {

    public:
        /// Create a new energy measure plugin
        /// @param[in]  app the Maci application instance
        /// @param[in]  mask    mask value
        /// @param[in]  outfile name of the output file
        MeasureEnergy(Maci* app, int mask, const char* outfile);

        /// Copy constructor
        /// @param[in]  o   the other instance
        MeasureEnergy(const MeasureEnergy &o);

        /// Clone function
        MeasureEnergy* clone() const;

        /// Destructor
        ~MeasureEnergy();

        /// Measure the base-level for the energy
        void setup();

        /// Measure and write the energy
        void post_step();

        /// Close the output file
        void post_integration();


    private:
        /// The mask is used to decide at which timesteps the measurements
        /// should be done. Only timesteps which are a multiple of the mask are
        /// considered.
        int         mask;
        /// Current timestep
        long        step;
        /// The MD interface allows for the calculation of the energy
        /// of a subset of the particles. 
        enum 
        {
            /// Calculate the energy of all particles
            PARTICLES_SELECT_ALL   = 0,
            /// Calculate only the energy of particles outside the
            /// coupling region ignoring all other particles in the
            /// calculation
            PARTICLES_SELECT_NONHS = 1,
            /// Calculate only the eneryg of particles in a box
            PARTICLES_SELECT_BOX   = 2
        } pselect;
        /// The name of the output file
        std::string outname;
        /// The file handle for the output file. This one is only valid
        /// on the root processes of the individual levels
        FILE*       outfile;
        /// Particle box. This pointer is only valid if pselect equals
        /// PARTICLES_SELECT_BOX
        polylib::Box<double, DIM>* box;

        /// Create a particle list according to the subset specifier
        ParticleList createParticleList();

        /// Measure energy
        /// @param[in]  n   step number
        void measureEnergy(int n);
    };

}

#endif

