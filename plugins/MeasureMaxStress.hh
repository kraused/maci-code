
// vi: tabstop=4:expandtab

#ifndef MACI_PLUGINS_MEASURE_MAX_STRESS_HH_INCLUDED
#define MACI_PLUGINS_MEASURE_MAX_STRESS_HH_INCLUDED 1

#include "Plugin.hh"
#include "ParticleList.hh"

#include <string>
#include <stdio.h>

namespace plugins {

    /// MeasureMaxStress: A plugin for measuring maximal stresses in the
    /// atomistic system
    class MeasureMaxStress : public Plugin
    {

    public:
        /// Create a new energy measure plugin
        /// @param[in]  args    the arguments seperated by ;
        MeasureMaxStress(const char *args);

        /// Copy constructor
        /// @param[in]  o   the other instance
        MeasureMaxStress(const MeasureMaxStress &o);

        /// Clone function
        MeasureMaxStress* clone() const;

        /// Do nothing
        void start() { }

        /// Closes the output file
        void stop ();

        /// Measures energy
        void apply();

    private:
        /// The strategy
        enum {
            /// Choose stresses in the vicinity of the maximal stress 
            STRATEGY_MAX  = 0
        } strategy;
        /// 
        double eta;
        /// Stress treshold value
        double treshold;
        /// The maximal stress value
        double maxStress;
        /// The prefix for output files
        std::string prefix;
        /// Output file for the maximal stresses (over time)
        FILE *maxStressOutfile;

//      /// Measure the maximal stress.
//      ///
//      /// Since the stress of a single atom is a not a meaningfull quantity
//      /// we need to some form of clustering and averaging to compute meaningfull
//      /// averages.
//      /// Currently we take the simpler approach and print out the particles
//      /// with the stresses larger than a threshold value. This allows to do
//      /// some analysis as post-processing
//      ///
//      /// @param[in]  n   the step
//      void Measure_Max_Stress(int n);

    };

}

#endif

