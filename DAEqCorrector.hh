
// vi: tabstop=4:expandtab

#ifndef MACI_DAEQ_CORRECTOR_HH_INCLUDED
#define MACI_DAEQ_CORRECTOR_HH_INCLUDED 1

#include "Pointers.hh"
#include "ResidualNormComputer.hh"


/// Forwarding
class Maci;


/// DAEqCorrector: Base class for all correctors in the
///                integrators for the differential-algebraic
///                equations of motion
class DAEqCorrector : public Pointers
{

public:
    /// Create a new instance
    /// @param[in]  app the Maci application instance
    DAEqCorrector(Maci* app);

    /// Factory function for ResidualNormComputer. Initially the design was so
    /// that DAEqCorrector itself provided a way to compute the norm. However,
    /// using a seperate ResidualNormComputer instance seems to be a better way.
    /// The problem, however, is that R and M are not globally accessible and therefor
    /// plugins like MonitorResidual which use ResidualNormComputer cannot create
    /// the instance themeself. To overcome this problem we have added a factory
    /// function.
    virtual ResidualNormComputer* NewResidualNormComputer() = 0;

public:
    /// Time spend in the correction
    double time_spend;
};

#endif

