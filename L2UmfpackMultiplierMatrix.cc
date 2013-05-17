
// vi: tabstop=4:expandtab

#include "L2UmfpackMultiplierMatrix.hh"
#include "L2Assembler.hh"
#include "FETruncMassAssembler.hh"
#include "MassTimesTransposedTransferAssembler.hh"
#include "MultiplierAssembler.hh"
#include "linalg/SerialUmfpackSolver.hh"
#include "Int64.hh"
#include "DesignByContract.hh"
#include "Maci.hh"


L2UmfpackMultiplierMatrix::L2UmfpackMultiplierMatrix(Maci* app, const float h[])
: UmfpackMultiplierMatrix(app)
{
    TreeAssembler *t;
    MassTimesTransposedTransferAssembler *q;

    if(0 == getSecAssembler("L2Transfer"))
        addSecAssembler("L2Transfer", new L2Assembler(*this->handshake,h));
    
    if(0 == getSecAssembler("FETruncLumpedMasses"))
        addSecAssembler("FETruncLumpedMasses", new FETruncMassAssembler(*this->handshake))
    
    t = dynamic_cast<TreeAssembler*>(getSecAssembler("L2Transfer"));

    if(0 == getSecAssembler("L2MassTimesTransposedTransfer")) 
    {
        ParticleList list;
        if(PROC_MD == gcpe()->mytype)
            list.AddSelection(SelectHandshakeParticles());

        addSecAssembler("L2MassTimesTransposedTransfer", new MassTimesTransposedTransferAssembler(t, list));
    }
    
    q = dynamic_cast<MassTimesTransposedTransferAssembler*>(getSecAssembler("L2MassTimesTransposedTransfer"));

    CHECK_ALWAYS(t, 0);
    CHECK_ALWAYS(q, 0);

    addPriAssembler("L2MultiplierMatrix", new MultiplierAssembler(t, q, getSecAssembler("FETruncLumpedMasses"), ));

    createSolver(dynamic_cast<MultiplierAssembler*>(ass));
}

