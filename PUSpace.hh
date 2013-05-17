
// vi: tabstop=4:expandtab

#ifndef MACI_PUSPACE_HH_INCLUDED
#define MACI_PUSPACE_HH_INCLUDED 1

#include "PUSpaceBase.hh"
#include "pulib/pulib.hh"


/// A partition of unity approximation space on the particles in the handshake
/// region.
class PUSpace : public PUSpaceBase, public pulib::ApproximationSpace<DIM>
{
    typedef PUSpaceBase Base;
    typedef pulib::ApproximationSpace<DIM> ApproxSpace;

public:
    /// Create an approximation space using the particles in the handshake
    /// region
    /// @param hs   the handshake geometry
    /// @param h    the patch size (same for all particles)
    PUSpace(const HandshakeGeo& hs, const float h[])
    : Base(hs, h), ApproxSpace(Base::communicator->getHandle(),
                               Base::nbranks, *Base::box, Base::pts, 
                               Base::hdiams, Base::ids, Base::N)
    {
    }

    // SWIG gets confused ...
#ifndef SWIG
    /// Create an approximation space from particle list
    /// @param list the particle list
    /// @param h    the patch size (same for all particles)
    PUSpace(const ParticleList& list, const float h[])
    : Base(list, h), ApproxSpace(Base::communicator->getHandle(),
                                 Base::nbranks, 
                                *Base::box, 
                                 Base::pts, 
                                 Base::hdiams, 
                                 Base::ids, 
                                 Base::N)
    {
    }
#endif

    /// Evaluate a basis function
    /// @param phiJ basis function to evaluate
    /// @param x    evaluation point
    /// @returns    value of basis function
    static double value(const pulib::BasisFunction<DIM>& phiJ, const double x[])
    {
        return pulib::ShepardEvaluator<DIM,pulib::TensorBSpline<DIM,1>/**/>::value(phiJ,x);
    }

};

#endif

