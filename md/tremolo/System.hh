
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_TREMOLO_SYSTEM_HH_INCLUDED
#define MD_INTERFACE_TREMOLO_SYSTEM_HH_INCLUDED 1

#include "md/tremolo/src/tremoloInstance.hh"

#include "md/tremolo/ParticleHandle.hh"
#include "md/GenericSystem.hh"
#include "md/Writer.hh"

namespace md {
    namespace interface {
        
        /// class which contains all the state information about the
        /// MD system, i.e. positions, velocities, ...
        class System : public GenericSystem<tremolo::Instance,double>
        {
            typedef GenericSystem<tremolo::Instance,double> Base;
        
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
                return GetImplementation()->NlocalAtoms();
            }

            /// Helper function to implement the operator++ for
            /// particles. The function jumps to the next linked 
            /// cell and returns the first particle in the list
            /// @param[inout]   ijk the current linked list
            ///             coordinates are updated
            ///             if the end of the 
            ///             particle list is reached.
            ParticlePtr NextParticle(int (*ijk)[3]) const;
            
            /// Execute a statement for all particles
            /// @param[in]  s   the statement; The class S must 
            ///         implement the operator
            ///         <code>
            ///         operator()(const ParticleHandle &) const
            ///         </code>
            template<class S>
            void foreach(const S &s) const
            {
                for(ParticleHandle ph = begin(); ph != end(); ++ph) 
                {
                    s(ph);
                }
            }

            /// Starts an access epoche.
            /// param @type defines the access pattern of the
            ///     phase. The behaviour is undefined if
            ///     other quantities are access
            inline void AccessBegin(int type)
            {
                epoch_type = type;
                this->GetImplementation()->AccessBegin(type);
            }
            
            /// Ends an access epoche
            inline void AccessEnd()
            {
                this->GetImplementation()->AccessEnd();
                epoch_type = INVALID;
            }
            
            /// Query epoche state
            /// returns true if an epoche is started. If type is
            /// not null, on exit it contains the type of the
            /// epoche
            inline bool CanAccess(int* type = 0) const
            {
                if(type) 
                    *type = epoch_type;
                
                return this->GetImplementation()->CanAccess();
            }
            
            /// Query the bounding box of the processing element
            /// in case a domain decomposition parallelization
            /// approach is used.
            /// This function can throw a FunctionNotSupportedException
            /// if the underlying implementation uses a different
            /// parallelization method or the 
            void GetSpatialRegion(double* lower, double* upper) const;
            
            /// Returns the communicator of the MD implementation
            inline MPI_Comm GetCommunicator() const
            {
                return this->GetImplementation()->comm;
            }

            /// Write atom data to a file
            /// @param[in]  file    The prefix of the output file. The suffix
            ///                     is appended by the writer
            void Write(const std::string& file);

        private:
            /// The access type
            /// This is an int rather than of type AccessType since
            /// we can | or & to define the type 
            int epoch_type;
            /// The writer
            Writer* writer;
        };
    
    }
}

#endif

