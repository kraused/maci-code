
// vi: tabstop=4:expandtab

#ifndef MACI_RATTLE_CORRECTOR_HH_INCLUDED
#define MACI_RATTLE_CORRECTOR_HH_INCLUDED 1

#include "DAEqCorrector.hh"
#include "NodalValueBuffer.hh"
#include "MexicoUser.hh"

#include "mpi/Communicator.hh"


/// Forwarding
class Maci;
class TransferOperator;
class MeshMassMatrix;
class MultiplierMatrix;
class ParticleExchangeBuffer;
class LinearResidual;
class DynCommunicationGraph;
class ParticleList;


/// RattleCorrector: A corrector for the RATTLE algorithm
class RattleCorrector : public DAEqCorrector, public MexicoUser
{
    
public:
    /// Create a new instance
    /// @param[in]  tau timestep size of the RATTLE time integrator
    /// @param[in]  app the Maci application instance
    /// @param[in]  R   the transfer operator used in the computation of the
    ///                 (averaged) constraints
    /// @param[in]  constraintsM    the mass matrix in the constraints
    /// @param[in]  correctionM     the correction matrix for mesh processing
    ///                             elements.
    /// @param[in]  L   The multiplier matrix. This matrix must be inverted
    ///                 to obtain the Lagrange forces
    RattleCorrector(Maci* app, double tau, TransferOperator* R, MeshMassMatrix* constraintsM, MeshMassMatrix* correctionM, MultiplierMatrix* L);

    /// Destructor
    ~RattleCorrector();

    /// Correct the displacements and velocities after the first kick and the
    /// drift in the RATTLE integrator
    /// @param[inout] mu            the Lagrange multiplicator
    void CorrectAfterKickDrift(NodalValueBuffer<double>* lambda);

    /// Correct the displacements and velocities after the first kick and the
    /// drift in the RATTLE integrator
    /// @param[in]    list      the particles in the handshake region
    /// @param[inout] mu        the Lagrange multiplicator
    void CorrectAfterKickDrift(ParticleList* list, NodalValueBuffer<double>* lambda);

    /// Correct the velocities after the second kick in the RATTLE time step
    /// @param[inout] lambda        the Lagrange multiplier
    void CorrectAfterKick(NodalValueBuffer<double>* mu);

    /// Correct the velocities after the second kick in the RATTLE time step
    /// @param[in]    list      the particles in the handshake region
    /// @param[inout] lambda    the Lagrange multiplier
    void CorrectAfterKick(ParticleList* list, NodalValueBuffer<double>* mu);

    /// Factory function for ResidualNormComputer. Initially the design was so
    /// that RattleCorrector itself provided a way to compute the norm. However,
    /// using a seperate ResidualNormComputer instance seems to be a better way.
    /// The problem, however, is that R and M are not globally accessible and therefor
    /// plugins like MonitorResidual which use ResidualNormComputer cannot create
    /// the instance themeself. To overcome this problem we have added a factory
    /// function.
    ResidualNormComputer* NewResidualNormComputer();

private:
    /// The timestep size
    double tau;
    /// The TransferOperator instance is used in the computation
    /// of the constraints to transfer the MM displacements or velocities
    /// onto the finite element mesh
    TransferOperator* R;
    /// Constraints mass matrix
    MeshMassMatrix* constraintsM;
    /// The inverse weighted finite element masses times the transposed
    /// constraintsM
    MeshMassMatrix* correctionM;
    /// The MultiplierMatrix instance L must be inverted twice in each
    /// timestep to compute the Lagrange forces acting on both systems
    MultiplierMatrix* L;
    /// The linear residual
    NodalValueBuffer<double>* g;
    /// The FE displacements or velocities stored in a buffer
    NodalValueBuffer<double>* u;
};

#endif

