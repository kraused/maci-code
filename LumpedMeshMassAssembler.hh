
// vi: tabstop=4:expandtab

#ifndef MACI_LUMPED_MESH_MASS_ASSEMBLER_HH_INCLUDED
#define MACI_LUMPED_MESH_MASS_ASSEMBLER_HH_INCLUDED 1

#include "MeshMassAssembler.hh"
#include "NodeList.hh"
#include "ElementList.hh"


/// LumpedMeshMassAssembler: Base class for all mesh mass
///                          assembler which create lumped
///                          approximations
/// It is common practice to lump the mass matrix in time-integration
/// schemes to get rid of the need to invert the matrix in
/// each time-step. 
///
/// TODO In the current code version (15/01/11) we cannot use
///      non-lumped mass matrices in the constraints either. This
///      is because the SpMV product would require communication.
///      The mechanisms for this are currently not implemented
///
class LumpedMeshMassAssembler : public MeshMassAssembler
{

public:
    /// Assemble the output matrix taking into account all
    /// the elements in list
    void AssembleMatrix(ElementList& list, Function<double, 3>& f);

    /// Create the output matrix from the fe::interface::StateVariables::Mass()
    /// values. Only nodes in list are taken into account
    void AssembleMatrixUsingFEMass(NodeList& list);

    /// Create the output matrix from the fe::interface::StateVariables::WeightedMass()
    /// values. Only nodes in list are taken into account
    void AssembleMatrixUsingFEWeightedMass(NodeList& list);

};

#endif

