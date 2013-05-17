
// vi: tabstop=4:expandtab

#ifndef MACI_DAMPING_METHOD_HH_INCLUDED
#define MACI_DAMPING_METHOD_HH_INCLUDED 1

#include <valarray>

#include "ProcElement.hh"
#include "ParticleList.hh"
#include "Pointers.hh"

#include "plugins/MonitorDampingForce.hh"

#include "md/MolecularDynamics.hh"

/// Forwarding
class Maci;


/// DampingMethod: Base class for all damping technique
///                implementations
class DampingMethod : public Pointers
{

public:
    
    /// Constructor
    /// @param[in]  app the Maci application instance
    DampingMethod(Maci* app);
    
    /// Apply the damper to the particle system. This modifies
    /// the forces.
    void Apply()
    {
        using namespace md::interface;

        int i;
        double t0, t1;
        System* sys = this->system;

        t0 = MPI_Wtime();

        /// The number of particles on this processor changes
        /// over time so it might be necessary to resize it
        /// when new particles are added to the list of local
        /// particles
        F.resize(3*sys->NlocalAtoms());
        F = 0.0;

        /// Implemented by the derived class
        ComputeDissipativeForce(&F);

        sys->AccessBegin(System::RD_FORCE | System::WR_FORCE);

        /// Add the force F to the particle forces.
        for(ParticleHandle ph = sys->begin(); ph != sys->end(); ++ph)
            for(i = 0; i < DIM; ++i)
                Property::Force(*sys,ph,i) += F[3*ph.GetLocalId()+i];

        sys->AccessEnd();

        t1 = MPI_Wtime();
        time_spend += t1 - t0;  /// Sum them up: The ShowProgress plugin
                                /// is computing the average
    }

    /// Compute the dissipative force
    /// @param[out] F       the force term to be added to the particle
    ///                     forces
    virtual void ComputeDissipativeForce(std::valarray<double>* F) = 0;

    /// Time spend in the Apply() function
    double time_spend;

private:
    /// The force change which is added to the MD forces
    /// to introduce dissipation
    std::valarray<double> F;

    /// Make the MonitorDampingForce class a friend. It must have
    /// access to F
    friend class plugins::MonitorDampingForce;

};

#endif

