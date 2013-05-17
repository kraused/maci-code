
// vi: tabstop=4:expandtab

#ifndef MD_LAMMPS_ENERGY_HH_INCLUDED
#define MD_LAMMPS_ENERGY_HH_INCLUDED 1

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
            double K = 0;

            sys.GetImplementation()->computes().Compute_ke();

            for(; begin != end; ++begin)
                    K += Property::KineticEnergy(sys, *begin);

            MPI_Allreduce(MPI_IN_PLACE, &K, 1, MPI_DOUBLE, MPI_SUM, sys.GetCommunicator());
            return K;
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
            double P = 0;

            sys.GetImplementation()->computes().Compute_pe();

            for(; begin != end; ++begin)
                P += Property::PotentialEnergy(sys, *begin);

            MPI_Allreduce(MPI_IN_PLACE, &P, 1, MPI_DOUBLE, MPI_SUM, sys.GetCommunicator());
            return P;
        }       

    }
}

#endif

