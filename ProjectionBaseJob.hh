
// vi: tabstop=4:expandtab

#ifndef MACI_PROJECTION_BASE_JOB_HH_INCLUDED
#define MACI_PROJECTION_BASE_JOB_HH_INCLUDED 1

#include "NodalValueBuffer.hh"
#include "TransferOperator.hh"

#include "mexico/mexico.hpp"


/// ProjectionBaseJob: Job used by ProjectionBase::project
class ProjectionBaseJob : public mexico::Job
{

public:
    /// Constructor
    ProjectionBaseJob(TransferOperator* P, NodalValueBuffer<double>* u);
    
    /// Perform the SpMV
    void exec(void* i_buf, void* o_buf);

public:
    /// The projection matrix. The matrix is distributed
    /// by rows and each row is stored on the processing element
    /// which initially holds the corresponding atom.
    TransferOperator* P;
    /// Output buffer
    NodalValueBuffer<double>* u;

};

#endif

