
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_TREMOLO_ENERGY_HH_INCLUDED
#define MD_INTERFACE_TREMOLO_ENERGY_HH_INCLUDED 1

#include "md/tremolo/src/tremoloInstance.hh"
/* TODO Don't use the tremolo implementation directly */
#include "treLJEnergy.hh"
#include "md/tremolo/ParticleHandle.hh"

namespace md {
    namespace interface {

        /// Compute the kinetic energy of a a set of particles
        /// @tparam Iterator    iterator class which gives a particle
        ///         handle when being dereferenced
        /// @param  sys     the particle system
        /// @param  begin   the start iterator
        /// @param  end     end of the list
        /// @tparam F       a functor which implements the function
        ///         <code>
        ///         double operator()(const ParticleHandle &ph) const
        ///         </code>
        ///         returning the weight for a particle
        /// @param  f       functor instance 
        template<typename Iterator, typename F>
        double KineticEnergy(System& sys, Iterator begin, Iterator end, const F &f)
        {
            sys.AccessBegin(System::RD_PIGGYBACK |
                    System::RD_VEL       |
                    System::RD_MASS);

            double K = 0.0;
            for(; begin != end; ++begin) {
                double v = 0.0, w;
                for(int d = 0; d < 3; ++d) {
                    w  = Property::Vel(sys,*begin,d);
                    v += w*w;
                }

                K += Property::Mass(sys, *begin)*f(*begin)*v;
            }

            sys.AccessEnd();

            MPI_Allreduce(MPI_IN_PLACE, &K, 1, MPI_DOUBLE, MPI_SUM,
                      sys.GetCommunicator());

            return 0.5*K;
        }

        /// Compute the potential energy of a set of particles
        /// @tparam Iterator    iterator class which gives a particle
        ///         handle when being dereferenced
        /// @param  sys     the particle system
        /// @param  begin   the start iterator
        /// @param  end     end of the list
        /// @tparam F       a functor which implements the function
        ///         <code>
        ///         double operator()(const ParticleHandle &ph) const
        ///         </code>
        ///         returning the weight for a particle
        /// @param  f       functor instance 
        template<typename Iterator, typename F>
        double PotentialEnergy(System& sys, Iterator begin, Iterator end, const F &f)
        {
            // f can be ignored since it is part of the Tremolo
            // implementation

            // No need to start an access epoche since the transformation
            // into real coordinates is handled by the implementation

            for(Iterator i = begin; i != end; ++i) {
                i->MarkForEnergyEvaluation();
            }

            LJEvaluatePotentialEnergy(sys.GetImplementation());

            double V = 0.0;
            for(Iterator i = begin; i != end; ++i) {
                V += i->GetEnergy();
                i->UnmarkForEnergyEvaluation();
            }

            MPI_Allreduce(MPI_IN_PLACE, &V, 1, MPI_DOUBLE, MPI_SUM, sys.GetCommunicator());

            return V;
        }

    }
}

#endif

