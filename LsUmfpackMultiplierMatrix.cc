
// vi: tabstop=4:expandtab

#include "LsUmfpackMultiplierMatrix.hh"
#include "LsAssembler.hh"
#include "FELsTruncMassMatrixAssembler.hh"
#include "MassTimesTransposedTransferAssembler.hh"
#include "MultiplierAssembler.hh"
#include "linalg/SerialUmfpackSolver.hh"
#include "Int64.hh"
#include "DesignByContract.hh"
#include "Maci.hh"

LsUmfpackMultiplierMatrix::LsUmfpackMultiplierMatrix(Maci* app) 
: UmpfackMultiplierMatrix(app)
{
    TreeAssembler *t;
    MassTimesTransposedTransferAssembler *q;

    if(0 == GetSecondaryAssembler("LsTransfer"))
        AddSecondaryAssembler("LsTransfer", new LsAssembler(*this->handshake));
    
    t = dynamic_cast<TreeAssembler*>(GetSecondaryAssembler("LsTransfer"));

    if(0 == GetSecondaryAssembler("FELsTruncLumpedMasses"))
        AddSecondaryAssembler("FELsTruncLumpedMasses", new FELsTruncMassMatrixAssembler(*this->handshake,t));
    
    if(0 == GetSecondaryAssembler("LsMassTimesTransposedTransfer")) 
    {
        ParticleList list;
        if(PROC_MD == gcpe()->mytype)
            list.AddSelection(SelectHandshakeParticles());

        AddSecondaryAssembler("LsMassTimesTransposedTransfer", new MassTimesTransposedTransferAssembler(t, list));
    }
    
    q = dynamic_cast<MassTimesTransposedTransferAssembler*>(GetSecondaryAssembler("LsMassTimesTransposedTransfer"));

    CHECK_ALWAYS(t, 0);
    CHECK_ALWAYS(q, 0);

    AddPrimaryAssembler("LsMultiplierMatrix", new MultiplierAssembler(t, q, GetSecondaryAssembler("FELsTruncLumpedMasses")));

    createSolver(dynamic_cast<MultiplierAssembler*>(ass));
}


