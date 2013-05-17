
// vi: tabstop=4:expandtab

#include "InterpolationAssembler.hh"
#include "md/MolecularDynamics.hh"


/// Since we use the partition of unity tree we need to specify a patch size.
/// We use tiny patches so that the cut detection between patches is very
/// fast since they don't overlap at all
static float leastSquaresPatchSize[] = { 1e-1, 1e-1, 1e-1 };

InterpolationAssembler::InterpolationAssembler(const HandshakeGeo& hs)
: TreeAssembler(hs, leastSquaresPatchSize)
{
}

InterpolationAssembler::~InterpolationAssembler()
{
}

void InterpolationAssembler::assemble()
{
    AssembleIntermediateMatrix();
    CreateConsistentCoordinateMatrix();
    DeleteIntermediateMatrix();

    /// Transpose the matrix
    matrix.Transpose();
}

