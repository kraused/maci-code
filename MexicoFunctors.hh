
// vi: tabstop=4:expandtab

#ifndef MACI_COMMLIB_FUNCTORS_HH_INCLUDED
#define MACI_COMMLIB_FUNCTORS_HH_INCLUDED 1

#include "md/MolecularDynamics.hh"


/// In this file we provide a set of Functor classes which
/// can be put together in a plug-and-play fashion to 

/// MexicoFunctor_i3o3: A functor for the case that i_cnt and o_cnt 
///                      are 3 and 3, resp.
/// Since most jobs either want velocities or displacements as input and 
/// output vectors again this is the most common form
struct MexicoFunctor_i3o3
{

    static int i_cnt()
    {
        return 3;
    }

    static int o_cnt()
    {
        return 3;
    }

};


/// MexicoFunctor_fn: A building block for the case that no communication of data from
///                    the worker is wanted
struct MexicoFunctor_fn
{

    inline int num_o_worker(md::interface::System* system, const md::interface::ParticleHandle& ph) const
    {
        return 0;
    }

    inline int o_worker(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        CHECK_ALWAYS(1 == 0, 0);
    }

    inline int o_offset(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        CHECK_ALWAYS(1 == 0, 0);
    }

};


/// MexicoFunctor_2f: A functor for communication to the FE processing elements
/// This "building block" can be used for the communication of data to the FE processing
/// elements. The class uses the fesource and fedispl classes as worker and offsets.
struct MexicoFunctor_2f
{

    inline int num_i_worker(md::interface::System* system, const md::interface::ParticleHandle& ph) const
    {
        int i, n;

        n = 0;
        for(i = 0; i < md::interface::PiggybackType::MAX_FEPROCS; ++i)
            n += (MPI_PROC_NULL != md::interface::Property::Piggyback(*system, ph)->feprocs[i]);

        return n;
    }

    inline int i_worker(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        return md::interface::Property::Piggyback(*system, ph)->feprocs[k];
    }

    inline int i_offset(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        return md::interface::Property::Piggyback(*system, ph)->fedispl[k];
    }

};


/// MexicoFunctor_2f_ff: A functor for the communication to the FE processing
///                       elements and back
/// This "building block" can be used for the communication of data to the FE
/// processing elements and retrieval in a symmetric fashion.
struct MexicoFunctor_2f_ff : public MexicoFunctor_2f
{
    
    inline int num_o_worker(md::interface::System* system, const md::interface::ParticleHandle& ph) const
    {
        return num_i_worker(system, ph);    /// Use the symmetry
    }

    inline int o_worker(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        return i_worker(system, ph, k);     /// Use the symmetry
    }

    inline int o_offset(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        return i_offset(system, ph, k);     /// Use the symmetry
    }
    
};


/// MexicoFunctor_2b_fb: A functor for the communication to the handshake
///                       processing elements
/// This "building" block is used by the FineFluctuationXXX damping classes. It
/// uses the bdprocs, origin and locIndex members of the piggyback struct
struct MexicoFunctor_2b_fb
{

    inline int num_i_worker(md::interface::System* system, const md::interface::ParticleHandle& ph) const
    {
        int i, n;

        n = 0;
        for(i = 0; i < md::interface::PiggybackType::MAX_BDPROCS; ++i)
            n += (MPI_PROC_NULL != md::interface::Property::Piggyback(*system, ph)->bdprocs[i]);

        return n;
    }

    inline int num_o_worker(md::interface::System* system, const md::interface::ParticleHandle& ph) const
    {
        return 1;
    }

    inline int i_worker(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        return md::interface::Property::Piggyback(*system, ph)->bdprocs[k];
    }

    inline int o_worker(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        return md::interface::Property::Piggyback(*system, ph)->origin;
    }

    inline int i_offset(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        return md::interface::Property::Piggyback(*system, ph)->bddispl[k];
    }

    inline int o_offset(md::interface::System* system, const md::interface::ParticleHandle& ph, int k) const
    {
        return md::interface::Property::Piggyback(*system, ph)->locIndex;
    }

};


/// MexicoFunctor_U: A functor for sending displacements
struct MexicoFunctor_U
{

    inline void pack(md::interface::System* system, const md::interface::ParticleHandle& ph, double* buf) const
    {
        buf[0] = md::interface::Property::Displ(*system, ph, 0);
        buf[1] = md::interface::Property::Displ(*system, ph, 1);
        buf[2] = md::interface::Property::Displ(*system, ph, 2);
    }

};


/// MexicoFunctor_V: A functor for sending velocities
struct MexicoFunctor_V
{

    inline void pack(md::interface::System* system, const md::interface::ParticleHandle& ph, double* buf) const
    {
        buf[0] = md::interface::Property::Vel(*system, ph, 0);
        buf[1] = md::interface::Property::Vel(*system, ph, 1);
        buf[2] = md::interface::Property::Vel(*system, ph, 2);
    }

};

#endif

