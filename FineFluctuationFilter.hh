
// vi: tabstop=4:expandtab

#ifndef MACI_FINE_FLUCTUATION_FILTER_HH_INCLUDED
#define MACI_FINE_FLUCTUATION_FILTER_HH_INCLUDED 1

#include <valarray>

#include "AssemblyProduct.hh"
#include "TauProfiling.hh"
#include "MexicoUser.hh"

#include "linalg/SparseMatrix.hh"
#include "mpi/Group.hh"


/// Forwarding
class Maci;


/// The filter matrix which allows to compute the high frequency part of the
/// molecular displacement field. The latter can be used e.g. in a PML or some
/// other damping technique.
class FineFluctuationFilter : public AssemblyProduct, public Pointers, public MexicoUser
{

public:
    /// Construct a new filter
    /// @param[in]  app the Maci application pointer
    FineFluctuationFilter(Maci* app);

    /// Destroy the instance
    ~FineFluctuationFilter();

    /// Create the matrix
    ///
    /// @warning This function destroys the assembler matrix! This is necessary
    ///          as the matrices can be huge (couple of GB) and we can't afford
    ///          to waste this memory
    void CreateMatrix();

    /// Compute the fine fluctuation field of the displacements or velocities
    /// (or whatever value is packed by f) and scatter these values to the
    /// particles
    /// @param list the list of handshake particles
    /// @param f    gather/scatter instance. See MexicoUser for a list of functions
    ///             to be implemented in Tp
    template<typename Tp>
    void compute(ParticleList list, const Tp& f)
    {
        TAU_PROFILE("FineFluctuationFilter::compute", "", TAU_DEFAULT);

        pack(list, f);
        execute_cli();
        scat(list, f);
    }

    /// A debugging function which computes the fine fluctuation field from
    /// the velocity values and stores it again in the velocities
    /// @param[in]  list        the particle list
    /// @param[in]  filename    name of the output file
    void computeAndStoreInVelocity(ParticleList& list, const char* filename);

private:
    /// The local matrix block. The matrix is distributed
    /// by rows and each row is stored on the processing element
    /// which initially holds the corresponding atom.    
    linalg::SparseMatrix<double>* matrix;

};

#endif

