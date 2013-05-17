
// vi: tabstop=4:expandtab

#ifndef MACI_PROJECTION_BASE_HH_INCLUDED
#define MACI_PROJECTION_BASE_HH_INCLUDED 1

#include "TransferOperator.hh"
#include "NodalValueBuffer.hh"
#include "ProjectionAssembler.hh"
#include "NodalValueBuffer.hh"
#include "MexicoUser.hh"

/// Forwarding
class Maci;


/// Base class for all projections
class ProjectionBase : public TransferOperator, public MexicoUser
{

public:
    /// Create a new instance
    ProjectionBase(Maci* app);

    /// Project the velocities
    /// @param plist    the list of particles on particle processing
    ///                 elements and 0 on mesh processing elements
    /// @param filename name of the output file. The prefix "fe-" or
    ///                 "md-" is prepended to make sure that file names
    ///                 are unique.
    void project(ParticleList *list, const char* filename);

private:
    NodalValueBuffer<double>* u;

};

#endif

