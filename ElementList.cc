
// vi: tabstop=4:expandtab

#include "ElementList.hh"
#include "HandshakeGeo.hh"

ElementList::ElementList()
: Base()
{
}

ElementList::ElementList(const HandshakeGeo& hs)
: Base(hs.elements())
{
}

void ElementList::addElement(const fe::interface::Element& el)
{
    Base::push_back(el);
}

