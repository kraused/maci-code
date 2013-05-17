
// vi: tabstop=4:expandtab

#ifndef MACI_HANDSHAKE_GEO_HH_INCLUDED
#define MACI_HANDSHAKE_GEO_HH_INCLUDED 1

#include "Pointers.hh"
#include "DesignByContract.hh"
#include "ProcElement.hh"
#include "ParticleList.hh"
#include "ElementList.hh"
#include "NodeList.hh"

#include "polylib/polylib.hh"
#include "md/MolecularDynamics.hh"
#include "hash/hash.hh"

#include <vector>


/// Forwarding
class Maci;


/// Abstract base class for all handshake regions
///
/// The class stores the list of particles in the handshake region, the list of
/// finite elements intersecting the handshake region and the list of nodes in
/// these finite elements. In general, these nodes must not lie inside the 
/// handshake region!
class HandshakeGeo : public Pointers
{

/* for SWIG */public:
    typedef polylib::Box<double,DIM>            Box;
    typedef md::interface::ParticleHandle       ParticleHandle;
    typedef fe::interface::Element              Element;
    typedef fe::interface::Node                 Node;

public:
    /// Returns true if the box b intersects
    /// the handshake geo. The method is allowed
    /// to return true also for boxes which do
    /// not intersect.
    virtual bool Intersects(const Box& b) const = 0;
    
    /// Currently this computes the intersection with the bounding box
    /// of the element
    virtual bool Intersects(const Element& e) const;
    
    /// Returns true if a point p is in the
    /// handshake region
    virtual bool IsInside(const double* p) const = 0;
    
    /// Make the compiler lucky ...
    virtual ~HandshakeGeo() { }
    
    /// Every processing element is either involved (i.e. its associated
    /// spatial region intersects the handshake region) or idle
    enum Status
    {
        INVOLVED  = 0,
        IDLE      = 1,
        UNDEFINED = 3
    };
    
    /// Default constructor
    /// @param[in]  app the Maci application instance
    HandshakeGeo(Maci* app);
                
    /// We cannot compute listP and listE in a constructor because this one
    /// would be called before any variables in the derived class (which
    /// has to implement the intersects function) can be initialized and
    /// therefore the function would always fail
    
    /// Update all internal caches for elements, nodes, ...
    /// The function modifies the bdcomm and bdlocal communicators and the
    /// bdremote group in here (need not be passed as an argument since 
    /// it is accessible via the gcpe() function
    /// This function assignes the bdIndex indices in the piggyback datastructure
    /// to the particles
    ///
    /// The function modifies to the node distribution manager of the
    /// finite element processing elements to make sure that handshake nodes
    /// are always owned by handshake processing elements
    ///
    void Update(/*ProcElement& here*/);

    /// Get reference to the particle list
    ParticleList& particles()
    {
        return listP;
    }

    /// Get reference to the particle list (const version)
    const ParticleList& particles() const
    {
        return listP;
    }
    
    /// Get reference to the element list
    ElementList& elements()
    {
        return listE;
    }

    /// Get reference to the element list (const version)
    const ElementList& elements() const
    {
        return listE;
    }
    
    /// Get reference to the node list
    NodeList& nodes()
    {
        return listN;
    }

    /// Get reference to the node list (const version)
    const NodeList& nodes() const
    {
        return listN;
    }

private:
    /// List of particles
    ParticleList listP;
    /// List of elements 
    ElementList  listE;
    /// List of nodes
    NodeList     listN;
    /// The status of the bd
    Status       status;
    
    /// Helper function for Update on FE processing elements
    void UpdateOnFE();
    /// Helper function for Update on MD processing elements
    void UpdateOnMD();

    /// Assign the "user-defined" node indices (Node::Index) so that the 
    /// nodes in the handshake region come first. Since many matrices range
    /// only over the handshake region nodes, having a contiguous index
    /// range improves locality and simplifies the storage.
    /// The function is called in Update(). It guarantees that the indices
    /// of handshake nodes span a contiguous range starting at zero.
    void AssignNodeIndices();

    /// Finite element codes typically use an element-based domain decomposition
    /// such that multiple copies of nodes are stored on different processing
    /// element. One of these processing elements is chosen to be the owner
    /// PE and the corresponding copy is called the primary copy. Data can be 
    /// copied from the primary copy to all other copies. The choice of whether
    /// a finite element processing element has status INVOLVED or IDLE is also
    /// based on the elements. Hence, we need to make sure that the primary copy
    /// of a node is located on a processing element with status INVOLVED. The
    /// function ChooseNodeOwners() is called in Update() to reassign the node
    /// ownership.
    void ChooseNodeOwners();

};

#endif

