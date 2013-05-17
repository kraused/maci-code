
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_LAMMPS_SYSTEM_HH_INCLUDED
#define MD_INTERFACE_LAMMPS_SYSTEM_HH_INCLUDED 1

#include "md/lammps/src/lammpsInstance.hh"

#include "md/lammps/ParticleHandle.hh"
#include "md/GenericSystem.hh"
#include "md/Writer.hh"

namespace md {
    namespace interface {

        /// class which contains all the state information about the
        /// MD system, i.e. positions, velocities, ...
        class System : public GenericSystem<lammps::Instance, double>
        {
            typedef GenericSystem<lammps::Instance,double> Base;

        public:
            /// True if the code employs a spatial decomposition
            /// parallelization approach
            static const bool UsesSpatialDecomp = true;
            

            /// Create a new instance. The implementation is
            /// <b>not</b> owned by the system class
            System(Implementation& Imp);
        
            /// Destructor
            ~System();

            /// Returns a handle for the first particle
            ParticleHandle begin() const;

            /// Returns a handle for the last particle
            ParticleHandle end() const;

            /// Return the number of local particles
            long NlocalAtoms() const
            {
                return GetImplementation()->numParticles();
            }

            /// Execute a statement for all particles
            /// @param s    the statement; The class S must implement
            ///     <code>
            ///     operator()(const ParticleHandle &) const
            ///     </code>
            template<class S>
            void foreach(const S &s) const
            {
                for(ParticleHandle ph = begin(); ph != end();
                    ++ph) {
                
                    s(ph);
                }
            }

            /// Starts an access epoch.
            /// param @type defines the access pattern of the
            ///     phase. The behaviour is undefined if
            ///     other quantities are access
            void AccessBegin(int type);

            /// Ends an access epoch
            void AccessEnd(); 

            /// Query epoch state
            /// returns true if an epoch is started. If type is
            /// not null, on exit it contains the type of the
            /// epoch
            bool CanAccess(int* type = 0);

            /// Query the bounding box of the processing element
            /// in case a domain decomposition parallelization
            /// approach is used.
            void GetSpatialRegion(double* lower, double* upper) const;
            
            /// Returns the communicator of the MD implementation
            inline MPI_Comm GetCommunicator() const
            {
                return cartworld;
            }
            
            /// Write atom data to a file
            /// @param[in]  file    The prefix of the output file. The suffix
            ///         is appended by the writer
            void Write(const std::string& file);

        private:
            /// Cartesified world communicator
            MPI_Comm cartworld;
            /// The writer used for I/O
            Writer* writer;

            /// The access type
            int epoch_type;
        };
                
    }
}

#endif

