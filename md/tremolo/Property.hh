
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_TREMOLO_PROPERTY_HH_INCLUDED
#define MD_INTERFACE_TREMOLO_PROPERTY_HH_INCLUDED 1

#include <assert.h>

#include "md/tremolo/PiggybackType.hh"

namespace md {
    namespace interface {
        
        /// collection of routines providing access to the properties
        /// of particles
        class Property
        {
        
        public:
            /// Returns the mass of a particle
            /// @param sys  the particle system
            /// @param ph   particle handle
            static double& Mass(const System& sys, const ParticleHandle& ph)
            {
                return ph._p->ParDat->mass;
            }

            /// Returns the position of a particle
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double& Pos(const System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(0 == d || 1 == d || 2 == d);
#ifndef NDEBUG
                {
                    int type;
                    ASSERT(sys.CanAccess(&type));
                    ASSERT(type & System::RD_POS || type & System::WR_POS);
                }
#endif


                return ph._p->x[d];
            }
            
            /// Returns the reference position of a particle
            /// @param[in]  sys the particle system
            /// @param[in]  ph  particle handle
            /// @param[in]  d   dimension
            static double RefP(const System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(0 == d || 1 == d || 2 == d);
#ifndef NDEBUG
                {
                    int type;
                    ASSERT(sys.CanAccess(&type));
                    ASSERT(type & System::RD_REFP);
                }
#endif

                return ph._p->x[3+d];
            }
            
            /// Returns the displacement of a particle
            /// @param sys  the particle system
            /// @param ph   particle handle
            /// @param d    dimension
            static double Displ(const System& sys, const ParticleHandle& ph, int d)
            {
#ifndef NDEBUG
                {
                    int type;
                    ASSERT(sys.CanAccess(&type));
                    ASSERT(type & System::RD_POS && type & System::RD_REFP);
                }
#endif

                return Pos(sys, ph, d) - RefP(sys, ph, d);
            }

            /// Returns the velocity of a particle
            /// @param[in]  sys the particle system
            /// @param[in]  ph  particle handle
            /// @param[in]   d  dimension
            static double& Vel(const System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(0 == d || 1 == d || 2 == d);
#ifndef NDEBUG
                {
                    int type;
                    ASSERT(sys.CanAccess(&type));
                    ASSERT(type & System::RD_VEL || type & System::WR_VEL);
                }
#endif

                return ph._p->u[d];
            }
            
            /// Returns the force of a particle
            /// @param[in]  sys the particle system
            /// @param[in]  ph  particle handle
            /// @param[in]  d   dimension
            static double& Force(const System& sys, const ParticleHandle& ph, int d)
            {
                ASSERT(0 == d || 1 == d || 2 == d);
#ifndef NDEBUG
                {
                    int type;
                    ASSERT(sys.CanAccess(&type));
                    ASSERT(type & System::RD_FORCE || type & System::WR_FORCE);
                }
#endif

                return ph._p->F[d];
            }
    
            /// Returns the piggyback data
            /// @param[in]  sys the particle system
            /// @param[in]  ph  particle handle
            static PiggybackType *Piggyback(System& sys, const ParticleHandle& ph)
            {
#ifndef NDEBUG
                {
                    int type;
                    ASSERT(sys.CanAccess(&type));
                    ASSERT(type & System::RD_PIGGYBACK || type & System::WR_PIGGYBACK);
                }
#endif

                return (PiggybackType *)&(ph._p->piggyback);
            }
        
            /// Returns the piggyback data (const version)
            /// @param[in]  sys the particle system
            /// @param[in]  ph  particle handle
            static const PiggybackType *Piggyback(const System& sys, const ParticleHandle& ph)
            {
#ifndef NDEBUG
                {
                    int type;
                    ASSERT(sys.CanAccess(&type));
                    ASSERT(type & System::RD_PIGGYBACK);
                }
#endif

                return (PiggybackType *)&(ph._p->piggyback);
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

//            /// Returns stress values
//            /// @param[in]  sys the particle system
//            /// @param[in]  ph  particle handle
//            /// @param[in]  x   x value
//            /// @param[in]  y   y value
//            static double& Stress(const System& sys, 
//                          const ParticleHandle& ph,
//                          int x, int y)
//            {
//                if(y < x) {
//                    std::swap(x,y);
//                }
//
//                int i = sys.GetImplementation()->measure_instance.ops[tremolo::BDT_STRESS].idx;
//                int k = x + y + (y > 0);
//
//                return ph._p->x[i + k];
//            }

        };

    }
}

#endif

