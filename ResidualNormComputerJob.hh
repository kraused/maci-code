
// vi: tabstop=4:expandtab

#ifndef MACI_RESIDUAL_NORM_COMPUTER_JOB_HH_INCLUDED
#define MACI_RESIDUAL_NORM_COMPUTER_JOB_HH_INCLUDED 1

#include "MeshMassMatrix.hh"
#include "NodalValueBuffer.hh"
#include "TransferOperator.hh"

#include "mexico/mexico.hpp"


/// ResidualNormComputerJob: Performs the computation of the residual
class ResidualNormComputerJob : public mexico::Job
{

public:
    /// Constructor
    ResidualNormComputerJob(TransferOperator* R, 
                            MeshMassMatrix* M, 
                            NodalValueBuffer<double>* u, 
                            NodalValueBuffer<double>* g);
    
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

};

#endif

