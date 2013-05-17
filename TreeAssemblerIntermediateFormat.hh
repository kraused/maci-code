
// vi: tabstop=4:expandtab

#ifndef MACI_TREE_ASSEMBLER_INTERMEDIATE_FORMAT_HH_INCLUDED
#define MACI_TREE_ASSEMBLER_INTERMEDIATE_FORMAT_HH_INCLUDED 1

#include <vector>

#include "DesignByContract.hh"
#include "ElementShadowList.hh"
#include "PlainOldData.hh"
#include "Int64.hh"


/// TreeAssemblerIntermediateFormat: The intermediate storage format
/// for the assembler. The template argument is the floating point type
/// to use
///
/// Why do we need this? - The way the assembling in Maci is implemented,
/// the tree search is performed on particle processing element. However,
/// we don't want particle processing elements to have a full copy of the
/// FE mesh, as the sending/receiving the connectivity information can be
/// quite complicated (MD and FE domain decomposition do not match in 
/// general). Hence, particle processing elements have only access to a
/// list of shadow elements which represent the original element. A shadow
/// element is similar to a proxy but does not provide the full functionality
/// of the original object. For example the shadow objects basically only contain
/// the corner coordinates. Hence, when assembling scale transfer matrices
/// we cannot build the [nodes] x [atoms] matrix but only a [elements] x [corners] x [atoms]
/// This is represented by the TreeAssemblerIntermediateFormat. After
/// having finished the tree search, the TreeAssemblerIntermediateFormat 
/// data is send back to the mesh processing elements which (using the mesh
/// connectivity information) can build the [nodes] x [atoms] matrix by merging
/// the entries in TreeAssemblerIntermediateFormat.
///
/// Storage format: The matrix is stored as an array of vector<Entry>. For each
/// element one vector<Entry> is created which is a COO format representation
/// of the sparse matrix A(e,:,:) in MATLAB notation.
///
template<typename Tp>
class TreeAssemblerIntermediateFormat
{

public:
    /// Element attached data
    struct Entry : public PlainOldData<Entry>
    {
        int elementIndex, cornerIndex;
        /// The local atom index: On mesh processing elements
        /// are of the form
        ///     +------+-------------+
        ///     | rank | local index |
        ///     +------+-------------+
        /// where rank is rank of the source processor in the graph
        /// communicator
        int64 atomIndex;
        /// The computed matrix value
        Tp val;
    };

    
    TreeAssemblerIntermediateFormat(ElementShadowList& els)
    : list(els), entries(new std::vector<Entry>[list.size()])
    {
    }

    /// Reserve storage for an element row.
    void reserve(int idx, long len)
    {
        CHECK_PRECOND_ALWAYS(idx >= 0 && idx < (int )list.size());
        CHECK_PRECOND_ALWAYS(len >= 0);
        CHECK_PRECOND_ALWAYS(entries);

        entries[idx].reserve(len);
    }

    /// Insert a new entry.
    inline void insert(ElementShadowList::Iterator el, int cornerIndex, int64 atomIndex, Tp val)
    {
        CHECK_PRECOND_ALWAYS(cornerIndex < el->ncorners);
        CHECK_PRECOND_ALWAYS(el-list.begin() >= 0 && el-list.begin() < (int )list.size());
        CHECK_PRECOND_ALWAYS(list.end()-el >= 0);

        Entry e;
        e.elementIndex = el->localIndex;
        e.cornerIndex  = cornerIndex;
        e.atomIndex    = atomIndex;
        e.val          = val;

        entries[el-list.begin()].push_back(e);
    }

private:
    /// Insert a new entry. This function is intended to be used on FE processing
    /// elements for inserting the retrieved values in the entries array. It only
    /// works on FE processing elements where (el-list.begin()) == e.elementIndex: 
    /// Since we don't have a reference to the ElementShadowList::Iterator object
    /// we cannot know where to insert the entry
    inline void insert(const Entry& e)
    {
        CHECK_PRECOND_DEBUG(PROC_FE == gcpe()->mytype);
        entries[e.elementIndex].push_back(e);
    }

public:
    /// Exchange the assembled values: The entries are send back to the FE processing
    /// elements which know the mesh connectivity and can assemble the full systems
    void Exchange();

    /// Entry iterator (const)
    typedef typename std::vector<Entry>::const_iterator Iterator;

    /// Get beginning of the Entry vector for the element el
    Iterator begin(const ElementShadow& el) const
    {
        return entries[el.localIndex].begin();
    }

    /// Get the end of the Entry vector for the element el
    Iterator end(const ElementShadow& el) const
    {
        return entries[el.localIndex].end();
    }

private:
    /// Reference to the shadow element list
    ElementShadowList&  list;
    /// Matrix entries: For each element we store one
    /// COO matrix.
    std::vector<Entry>* entries;
};

#endif

