
// vi: tabstop=4:expandtab

#ifndef MACI_ELEMENT_LIST_HH_INCLUDED
#define MACI_ELEMENT_LIST_HH_INCLUDED 1

#include <vector>

#include "ProcElement.hh"
#include "DesignByContract.hh"

#include "fe/FiniteElements.hh"

/// Forwarding
class HandshakeGeo;


/// ElementList: A list of elements, i.e. a subset of the finite element mesh
class ElementList : public std::vector<fe::interface::Element>
{
    typedef std::vector<fe::interface::Element> Base;

public:

    /// Create an empty list
    ElementList();

    /// Create an element list out of the array of handshake elements
    /// @param hs   the handshake geometry
    ElementList(const HandshakeGeo& hs);

    /// Add a new element
    /// @param el   the element to add
    void addElement(const fe::interface::Element& el);

    typedef Base::iterator       Iterator;
    typedef Base::const_iterator ConstIterator;

};

#endif

