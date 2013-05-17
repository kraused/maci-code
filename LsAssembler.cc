
// vi: tabstop=4:expandtab

#include "LsAssembler.hh"
#include "ProcElement.hh"
#include "md/MolecularDynamics.hh"


/// Since we use the partition of unity tree we need to specify a patch size.
/// We use tiny patches so that the cut detection between patches is very
/// fast since they don't overlap at all
static float leastSquaresPatchSize[] = { 1e-1, 1e-1, 1e-1 };

LsAssembler::LsAssembler(const HandshakeGeo& hs)
: TreeAssembler(hs, leastSquaresPatchSize)
{
    int i;

    if(PROC_MD == gcpe()->mytype) 
    {
        list.AddSelection(SelectHandshakeParticles());
        m.resize(list.size());

        gcpe()->GetMMSystem()->AccessBegin(md::interface::System::RD_MASS);

        i = 0;
        for(ParticleList::ConstIterator it = list.begin(); it != list.end(); ++it)
            m[i++] = md::interface::Property::Mass(*gcpe()->GetMMSystem(), *it);

        gcpe()->GetMMSystem()->AccessEnd();
    }
}

LsAssembler::~LsAssembler()
{
}

void LsAssembler::assemble()
{
    AssembleIntermediateMatrix();
    CreateConsistentCoordinateMatrix();
    DeleteIntermediateMatrix();
}

