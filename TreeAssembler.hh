
// vi: tabstop=4:expandtab

#ifndef MACI_TREE_ASSEMBLER_HH_INCLUDED
#define MACI_TREE_ASSEMBLER_HH_INCLUDED 1

#include "TreeAssemblerIntermediateFormat.hh"
#include "PUSpace.hh"
#include "ElementList.hh"
#include "ElementShadowList.hh"
#include "NodeList.hh"
#include "DesignByContract.hh"
#include "Assembler.hh"


/// TreeAssembler: A tree-based assembler for scale transfer matrices such
///                as Least-squares (or L2) projections. 
/// 
/// This assembler can be used to assemble transfer operators between particles
/// and finite elements. It internally handles a tree to do fast matching between
/// particles and elements. 
class TreeAssembler : public Assembler
{

protected:
    /// Functions that need to be implemented by derived classes:

    /// Query the storage requirements for the vector<Entry> in the 
    /// TreeAssemblerIntermediateFormat for element el. An upper bound
    /// is returned.
    virtual long QueryStorageRequirements(ElementShadowList::Iterator el) = 0;

    /// Assemble the matrix in its intermediate form
    virtual void BuildIntermediateMatrix(ElementShadowList::Iterator el, TreeAssemblerIntermediateFormat<double> *mat) = 0;

    /// Handle duplicate entries. Derived classes might choose to delete
    /// duplicate entries or to sum them up
    virtual void HandleDuplicates(linalg::CoordMatrix<int64,double>& mat) = 0;


public:
    /// Create a new instance
    TreeAssembler(const HandshakeGeo& hs, const float h[]);

    /// Destroy the instance
    virtual ~TreeAssembler();

    /// Assemble the intermediate matrix. The function calls 
    /// QueryStorageRequirements for all elements in the ElementShadowList,
    /// reserves the storage and calls BuildIntermediate(). It
    /// does not call HandleDuplicates()!
    void AssembleIntermediateMatrix();

    /// Delete the intermediate matrix to free space
    void DeleteIntermediateMatrix();

    /// Create the consistent coordinate matrix: The intermediate format is
    /// send back to the mesh processing elements and mapped from [elements] x [corners] x [atoms]
    /// to the output [nodes] x [atoms]. The final matrix will be stored on
    /// mesh processing elements.
    void CreateConsistentCoordinateMatrix();
 

protected:
    /// List of involved elements and nodes. These pointers are non-null
    /// only on FE processing elements
    ElementList* elements;
    NodeList* nodes;

    /// MD processing elements store only a "shadow" of the original mesh.
    /// Since FE processing elements must fill in the ElementShadowList,
    /// the pointer is valid on all processing elements
    ElementShadowList* ellist;

    /// Every tree assembler has it's own partition of unity space even if
    /// it doesn't use the partition of unity itself.
    /// The reason is that pulib implements all necessary algorithms on top
    /// of the tree library so that we should not duplicate the code here.
    /// If the partition of unity itself is of no interest, it is always possible
    /// to choose a small diameter of the support.
    PUSpace puspace;
    
    /// The intermediate storage for the matrix
    TreeAssemblerIntermediateFormat<double>* imat;
};

#endif

