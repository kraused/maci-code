
// vi: tabstop=4:expandtab

#ifndef MACI_NODE_LIST_HH_INCLUDED
#define MACI_NODE_LIST_HH_INCLUDED 1

#include "ProcElement.hh"
#include "DesignByContract.hh"

#include "fe/FiniteElements.hh"

#include <vector>

/// Forwarding
class HandshakeGeo;

/// A list of nodes
class NodeList : public std::vector<fe::interface::Node>
{
    typedef std::vector<fe::interface::Node> Base;

public:
    typedef fe::interface::Node Node;

    /// Create an empty list
    NodeList();

    /// Create the complete node list
    /// @param mesh the finite element mesh
    NodeList(const fe::interface::Mesh& mesh);

    /// Create a node list of out of the array of handshake nodes
    template<typename Selector>
    NodeList(const HandshakeGeo& hs, const Selector& f);

    /// Add all nodes between [begin, end) where the function f
    /// evaluates to true
    /// The functor selector class Selector should implement the function
    /// operator(Node& ) const
    template<typename Iterator, typename Selector>
    void AddSelection(Iterator begin, Iterator end, const Selector& f)
    {
        CHECK_PRECOND_ALWAYS(PROC_FE == gcpe()->mytype);
            
        while(begin != end) {
            if(f(*begin)) {
                addNode(*begin);
            }
            ++begin;
        }
    }

    /// Add a new node
    /// @param nd   the node to add
    void addNode(const Node& nd);

    typedef Base::iterator      Iterator;
    typedef Base::const_iterator    ConstIterator;

    /// Find a node by comparing vindices and return and iterator
    /// @param nd   the node pointer to find
    ConstIterator find(Node& nd) const;

    /// Sort the entries according to the value of Node::Index()
    void sort();

};

/// A selector functor which allows for choosing only the nodes owned by the
/// calling process
class SelectOwnedNodes
{

public:
    inline bool operator()(const NodeList::Node& nd) const
    {
        return gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(nd);
    }

};

/// A selector functor which allows for choosing all nodes
class SelectAllNodes
{

public:
    inline bool operator()(const NodeList::Node& nd) const
    {
        return true;
    }

};

#endif

