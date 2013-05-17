
// vi: tabstop=4:expandtab

#ifndef MACI_RATTLE_CORRECTOR_JOB_HH_INCLUDED
#define MACI_RATTLE_CORRECTOR_JOB_HH_INCLUDED 1

#include "MeshMassMatrix.hh"
#include "NodalValueBuffer.hh"
#include "MultiplierMatrix.hh"
#include "TransferOperator.hh"

#include "mexico/mexico.hpp"


/// RattleCorrectorJob: Performs the computation on the FE side in
///                     RattleCorrector
class RattleCorrectorJob : public mexico::Job
{

public:
    /// Constructor
    RattleCorrectorJob(TransferOperator* R, 
                       MeshMassMatrix* M, 
                       NodalValueBuffer<double>* u, 
                       NodalValueBuffer<double>* g, 
                       MultiplierMatrix* L);
    
    /// Perform the SpMV
    void exec(void* i_buf, void* o_buf);

public:
    /// The transfer matrix. The matrix is distributed
    /// by rows and each row is stored on the processing element
    /// which initially holds the corresponding atom.
    TransferOperator* R;
    /// The finite element mass matrix that shows up in
    /// the definition of the constraints
    MeshMassMatrix* M;
    /// The displacements or velocities on the FE side
    NodalValueBuffer<double>* u;
    /// The constraints. g is the right hand-side of the
    /// linear system to solve
    NodalValueBuffer<double>* g;
    /// The multiplier matrix
    MultiplierMatrix* L;
    /// The lagrange mutliplier computed by the job. (This is either
    /// lambda or mu)
    NodalValueBuffer<double>* nu;

};

#endif

