
// vi: tabstop=4:expandtab

#include <numeric>

#include "FineFluctuationFilterAssembler.hh"
#include "Int64.hh"
#include "DesignByContract.hh"
#include "Logging.hh"
#include "ProcElement.hh"
#include "ParticleList.hh"
#include "ParallelMatMul.hh"
#include "linalg/CoordMatrix.hh"


FineFluctuationFilterAssembler::FineFluctuationFilterAssembler(Assembler* interpolationAssembler,
                                                               Assembler* projectionAssembler)
: interpolationAssembler(interpolationAssembler), projectionAssembler(projectionAssembler)
{
}

void FineFluctuationFilterAssembler::assemble()
{
    std::size_t i;
    int myrank = gcpe()->world.myrank();

    ParticleList list;
    if(PROC_MD == gcpe()->mytype)
        list.AddSelection(SelectHandshakeParticles());

    /// Initialize (mat) equal to the unit matrix. On mesh processing
    /// elements and particle processing elements which do not touch
    /// the handshake, list.size() will be zero.
    for(i = 0; i < list.size(); ++i)
        matrix.insert(Int64::concat(myrank, i), Int64::concat(myrank, i), 1.0);

    /// Compute I - N*P
    ParallelMatMul(-1.0, interpolationAssembler->matrix, projectionAssembler->matrix, 1.0, &matrix);
}

