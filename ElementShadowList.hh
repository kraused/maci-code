
// vi: tabstop=4:expandtab

#ifndef MACI_ELEMENT_SHADOW_LIST_HH_INCLUDED
#define MACI_ELEMENT_SHADOW_LIST_HH_INCLUDED

#include "ElementShadow.hh"
#include "ElementList.hh"


/// ElementShadowList: A list of shadow elements
/// An instance of ElementShadowList contains all the information about
/// the finite element mesh that particle processing elements have access
/// to.
class ElementShadowList : public std::vector<ElementShadow>
{
    typedef std::vector<ElementShadow> Base;

public:
    /// Create an empty list
    ElementShadowList();

    /// Create a new list which is initialized by the members of a element
    /// list
    ElementShadowList(ElementList& list);

    /// Synchronize the element list between processors. 
    ///
    /// The currently implemented approach is to broadcast elements
    /// to all particle elements whether or not the bounding boxes
    /// intersect. This is not scalable but sufficient for the meshes we
    /// can deal with right now.
    ///
    void Exchange();

    /// Get an element pointer from the local index. This function
    /// can only be called on mesh processing elements.
    /// @param li       the local index
    /// @returns        reference to the element pointer
    inline fe::interface::Element& operator()(int li)
    {
        CHECK_PRECOND_DEBUG(ellist);
        CHECK_PRECOND_DEBUG(li >= 0 && li < (int )ellist->size());

        return (*ellist)[li];
    }

    /// Get an element pointer from the local index (const version)
    /// Works only on mesh processing elements.
    /// @param li       the local index
    /// @returns        const reference to the element pointer
    inline const fe::interface::Element& operator()(int li) const
    {
        CHECK_PRECOND_DEBUG(ellist);
        CHECK_PRECOND_DEBUG(li >= 0 && li < (int )ellist->size());

        return (*ellist)[li];
    }

    /// Iterators
    typedef Base::iterator       Iterator;
    typedef Base::const_iterator ConstIterator;

private:
    /// Pointer to the element list used for initialization. This
    /// allows for accessing the elements indirectly via the local 
    /// index
    ElementList* ellist;
};

#endif


