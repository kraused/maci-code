
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_LAMMPS_PROPERTY_HH_INCLUDED
#define MD_INTERFACE_LAMMPS_PROPERTY_HH_INCLUDED 1

#include <assert.h>

#include "md/lammps/PiggybackType.hh"

namespace md 
{
    namespace interface 
    {
        
        /// collection of routines providing access to the properties
        /// of particles
        class Property
        {
        
        public:
            /// Returns the mass of a particle
            /// @param sys  the particle system
            /// @param ph   particle handle
            static double Mass(const System& sys, const ParticleHandle& ph)
            {
                return sys.GetImplementation()->mass(ph.GetLocalId());
            }

            /// Returns the weighted mass of a particle
            /// @param sys  the particle system
            /// @param ph   particle handle
            static double WeightedMass(const System& sys, const ParticleHandle& ph)
            {
                return Piggyback(sys, ph)->weight*Mass(sys, ph);
            }

            /// Returns the mass of a particle as it shows up
            /// in the time integration algorithm. This can
            /// be a weighted or unweighted version of Mass()
            /// @param sys  the particle system
            /// @param ph   particle handle
            static double IntegratorMass(const System& sys, const ParticleHandle& ph)
            {
                /* TODO It might be a good idea to store the integrator mass
                    in the piggyback data to get rid of these if() statements
                    all over the place.
                 */
                if(sys.GetImplementation()->Wgt_enabled())
                    return WeightedMass(sys, ph);
                else
                    return Mass(sys, ph);
            }

            /// Returns the position of a particle
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double& Pos(System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(d >= 0 and d < 3);
                return sys.GetImplementation()->x(ph.GetLocalId())[d];
            }

            /// Returns the position of a particle (const version)
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double Pos(const System& sys,const ParticleHandle& ph, int d)
            {
                ASSERT(d >= 0 and d < 3);
                return sys.GetImplementation()->x(ph.GetLocalId())[d];
            }
            
            /// Returns the reference position of a particle
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double RefP(const System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(d >= 0 and d < 3);
                return sys.GetImplementation()->x0(ph.GetLocalId())[d];
            }
        
            /// Returns the displacement of a particle
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double Displ(const System& sys, const ParticleHandle& ph, int d)
            {
                return Pos(sys, ph, d) - RefP(sys, ph, d);
            }

            /// Returns the velocity of a particle (const version)
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double& Vel(System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(d >= 0 and d < 3);
                return sys.GetImplementation()->v(ph.GetLocalId())[d];
            }
            
            /// Returns the velocity of a particle (const version)
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double Vel(const System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(d >= 0 and d < 3);
                return sys.GetImplementation()->v(ph.GetLocalId())[d];
            }
            
            /// Returns the force of a particle
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double& Force(System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(d >= 0 and d < 3);
                return sys.GetImplementation()->f(ph.GetLocalId())[d];
            }
            
            /// Returns the force of a particle (const version)
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double Force(const System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(d >= 0 and d < 3);
                return sys.GetImplementation()->f(ph.GetLocalId())[d];
            }

            /// Returns the piggyback data
            /// @param sys  the particle system
            /// @param ph   particle handle
            static PiggybackType* Piggyback(System& sys, const ParticleHandle& ph)
            {
                return sys.GetImplementation()->pb(ph.GetLocalId());
            }

            /// Returns the piggyback data
            /// @param sys  the particle system
            /// @param ph   particle handle
            static const PiggybackType *Piggyback(const System& sys, const ParticleHandle& ph)
            {
                return sys.GetImplementation()->pb(ph.GetLocalId());
            }

            /// Returns the kinetic energy of the atom. This is an internal
            /// function: Implementations are not forced to provide or 
            /// expose this functionality
            /// @param sys  the particle system
            /// @param ph   particle handle
            static double KineticEnergy(System& sys, const ParticleHandle& ph)
            {
                return sys.GetImplementation()->computes().Query_ke(ph.GetLocalId());
            }

            /// Returns the potential energy of the atom. This is an internal
            /// function: Implementations are not forced to provide or 
            /// expose this functionality
            static double PotentialEnergy(System& sys, const ParticleHandle& ph)
            {
                return sys.GetImplementation()->computes().Query_pe(ph.GetLocalId());
            }

        };

    }
}

#endif

