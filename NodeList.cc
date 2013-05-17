
// vi: tabstop=4:expandtab

#include "NodeList.hh"
#include "HandshakeGeo.hh"

NodeList::NodeList()
: Base()
{
}

NodeList::NodeList(const fe::interface::Mesh& mesh)
{
    for(Node nd = mesh.NodesBegin(); nd != mesh.NodesEnd(); ++nd) {
        addNode(nd);
    }
}

template<typename Selector>
NodeList::NodeList(const HandshakeGeo& hs, const Selector& f)
{
    AddSelection(hs.nodes().begin(), hs.nodes().end(), f);
}

template
NodeList::NodeList(const HandshakeGeo& hs, const SelectOwnedNodes& f);
template
NodeList::NodeList(const HandshakeGeo& hs, const SelectAllNodes& f);

void NodeList::addNode(const Node& nd)
{
    Base::push_back(nd);
}

NodeList::ConstIterator NodeList::find(NodeList::Node& nd) const
{
    return std::find(begin(), end(), nd);
}

struct NodeIndexIsSmaller
{
    /// Returns true if the Node::Index() of nd1 is smaller than the
    /// one of nd2
    bool operator()(const NodeList::Node& nd1, const NodeList::Node& nd2)
    {
        return NodeList::Node::Index(*gcpe()->GetFESolver(), nd1) < NodeList::Node::Index(*gcpe()->GetFESolver(), nd2);
    }
};

void NodeList::sort()
{
    std::sort(begin(), end(), NodeIndexIsSmaller());
#ifndef NDEBUG
    for(std::size_t i = 1; i < size(); ++i)
        CHECK_DEBUG(Node::Index(*gcpe()->GetFESolver(), (*this)[i]) >= Node::Index(*gcpe()->GetFESolver(), (*this)[i-1]), 0);
#endif
}

