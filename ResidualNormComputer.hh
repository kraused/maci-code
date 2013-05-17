
// vi: tabstop=4:expandtab

#ifndef MACI_RESIDUAL_NORM_COMPUTER_HH_INCLUDED
#define MACI_RESIDUAL_NORM_COMPUTER_HH_INCLUDED 1

#include "MexicoUser.hh"
#include "Pointers.hh"
#include "NodalValueBuffer.hh"

/// Forwarding
class Maci;
class TransferOperator;
class MeshMassMatrix;


/// ResidualNormComputer: This class provides a way to compute the 
///                       residual norm
class ResidualNormComputer : public Pointers, public MexicoUser
{

public:
    /// Create a new instance
    /// The transfer matrix R and the finite element mass matrix M can
    /// be omitted by MM processing elements
    ResidualNormComputer(Maci* app, TransferOperator* R = 0, MeshMassMatrix* M = 0);

    /// Destructor
    ~ResidualNormComputer();

    /// Compute the norm of the linear displacement residual.
    double Nrm2U();

    /// Compute the norm of the linear velocity residual.
    double Nrm2V();

private:
    NodalValueBuffer<double> *u, *g;

};

#endif

