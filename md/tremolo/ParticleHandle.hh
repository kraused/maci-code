
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_TREMOLO_PARTICLEHANDLE_HH_INCLUDED
#define MD_INTERFACE_TREMOLO_PARTICLEHANDLE_HH_INCLUDED 1

#include "md/tremolo/src/tremoloInstance.hh"
/* TODO Don't use the tremolo implementation directly */
#include "treLJEnergy.hh"

namespace md {
    namespace interface {
    
        /// Forwarding
        class System;
        class Property;
    
        /// @see tremolo/data.h
        typedef Particle*   ParticlePtr;
    
        /// ParticleHandle: unique identifier for a particle
        /// A particle handle is a unique identifier for a particle; It
        /// is only valid as long as particle cells etc. are not updated
        class ParticleHandle
        {
        public:     
            /// Default constructor
            /// @param[in]  sys the system
            ParticleHandle(const System& sys);

            /// Create a particle handle from a pointer to a
            /// Tremolo particle
            /// @param[in]  sys the system
            /// @param[in]  p   the particle pointer
            /// @param[in]  ii  index
            /// @param[in]  ijk the cell index
            ParticleHandle(const System& sys, ParticlePtr p, int ii, const int* ijk = 0);

            /// Create a particle handle from a pointer to a
            /// Tremolo particle
            /// @param[in]  sys the system
            /// @param[in]  p   the particle pointer
            /// @param[in]  ii  index
            /// @param[in]  i,j,k   the cell indices
            ParticleHandle(const System& sys, ParticlePtr p, int ii, int i, int j, int k);
            
            /// Copy constructor
            /// @param[in]  ph  the other instance
            inline ParticleHandle(const ParticleHandle& ph)
            : _p(ph._p), idx(ph.idx), _sys(ph._sys)
            {
                std::copy(ph._ijk, ph._ijk+3, _ijk);
            }

            /// Get a locally unique id for the particle. The
            /// Ids are guaranteed to be contiguous between
            /// zero and the number of local particles. The ids
            /// are temporary and only valid until the particle
            /// distribution changes.
            inline int GetLocalId() const
            {
                return idx;
            }

            /// Get the globally unique particle index
            inline int GetGlobalId() const
            {
                return _p->Id;
            }
            
            /// Jump to the next particle
            ParticleHandle& operator++();

            // Check if two handles are not identical
            bool operator!=(const ParticleHandle& ph) const
            {
#ifndef NDEBUG
                if(_p == ph._p)
                    ASSERT(-1 == ph.idx || idx == ph.idx);
#endif

                return (     _p != ph._p     ) ||
                       (_ijk[0] != ph._ijk[0]) ||
                       (_ijk[1] != ph._ijk[1]) ||
                       (_ijk[2] != ph._ijk[2]);
            }
            
            // Check if two particle handles are identical
            bool operator==(const ParticleHandle& ph) const
            {
#ifndef NDEBUG
                                if(_p == ph._p)
                                        ASSERT(-1 == ph.idx || idx == ph.idx);
#endif

                return (     _p == ph._p     ) &&
                       (_ijk[0] == ph._ijk[0]) &&
                       (_ijk[1] == ph._ijk[1]) &&
                       (_ijk[2] == ph._ijk[2]);
            }
            
            // The assignment operator must be implemented since
            // otherwise we cannot store ParticleHandles in stl
            // containers
            // Warning: Assigning particle handles from different
            //      systems is not supported!
            inline ParticleHandle& operator=(const ParticleHandle& ph)
            {
                if(this != &ph) {
                    _p  = ph._p;
                    idx     = ph.idx;
                    _ijk[0] = ph._ijk[0];
                    _ijk[1] = ph._ijk[1];
                    _ijk[2] = ph._ijk[2];
                }
                return *this;
            }
            
            /// Mark the particle for energy evaluation
            /// @warning This is a function internal to the interface
            ///      implementation and should not be used from
            ///      outside since it might not be implemented for
            ///      other modules
            inline void MarkForEnergyEvaluation()
            {
                tremolo::LJEnergyMarkAtom(_p);
            }

            /// Unmark it again
            /// @warning This is a function internal to the interface
            ///      implementation and should not be used from
            ///      outside since it might not be implemented for
            ///      other modules
            inline void UnmarkForEnergyEvaluation()
            {
                tremolo::LJEnergyUnmarkAtom(_p);
            }

            /// Get the energy of the particle
            /// @warning This is a function internal to the interface
            ///      implementation and should not be used from
            ///      outside since it might not be implemented for
            ///      other modules
            inline double GetEnergy()
            {
                return _p->charge;
            }

        private:
            /// cell index
            int     _ijk[3];
            /// the tremolo particle
            ParticlePtr _p;
            /// The local index. We start at zero and increment the index
            /// when the ++ operator is called
            int     idx;
            /// A reference to the system. I need this for the
            /// implementation of the ++ operator
            const System&   _sys;
            
            /// All my friends ...
            friend class Property;
        };  
    }
}

#endif

