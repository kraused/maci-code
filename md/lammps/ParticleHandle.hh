
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_LAMMPS_PARTICLEHANDLE_HH_INCLUDED
#define MD_INTERFACE_LAMMPS_PARTICLEHANDLE_HH_INCLUDED 1

#include <stdlib.h>

namespace md {
    namespace interface {
    
        /// Forwarding
        class Property;
    
        /// a particle handle is a unique identifier for a particle
        class ParticleHandle
        {
        public: 
            /// Construct a new particle handle specifying the
            /// index
            /// @param i    the id
            ParticleHandle(size_t i)
            : pid(i)
            {
            }
            
            /// Copy constructor
            inline ParticleHandle(const ParticleHandle& ph)
            : pid(ph.pid)
            {
            }
            
            /// Get a locally unique id for the particle. The
            /// Ids are guaranteed to be contiguous between
            /// zero and the number of local particles. The ids
            /// are temporary and only valid until the particle
            /// distribution changes.
            inline int GetLocalId() const
            {
                return pid;
            }
            
            /// Get next particle
            inline ParticleHandle& operator++()
            {
                ++pid;
                return (*this);
            }
            
            /// Check if two handles are not identical
            bool operator!=(const ParticleHandle& ph) const
            {
                return (pid != ph.pid);
            }
            
            // Check if two particle handles are identical
            bool operator==(const ParticleHandle& ph) const
            {
                return (pid == ph.pid);
            }
            
            // The assignment operator must be implemented since
            // otherwise we cannot store ParticleHandles in stl
            // containers
            inline ParticleHandle& operator=(const ParticleHandle& ph)
            {
                if(this != &ph) {
                    pid = ph.pid;
                }
                return *this;
            }

        private:
            /// The particle index
            size_t  pid;
            
            /// All my friends ...
            friend class Property;
        };  
    }
}

#endif

