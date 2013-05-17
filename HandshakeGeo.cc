
// vi: tabstop=4:expandtab

#include <valarray>

#include "HandshakeGeo.hh"
#include "DesignByContract.hh"
#include "ElementBoundingBox.hh"
#include "Logging.hh"
#include "Maci.hh"


bool HandshakeGeo::Intersects(const Element& e) const
{
    return Intersects(ElementBoundingBox(e));
}

HandshakeGeo::HandshakeGeo(Maci* app)
: Pointers(app), status(UNDEFINED)
{
}

void HandshakeGeo::Update()
{
    switch(procel->mytype)
    {
    case PROC_FE:
        UpdateOnFE();
        break;
    case PROC_MD:
        UpdateOnMD();
        break;
    default:
        THROW(BadDefaultException, "");
    }

    /// Create the communicator
    procel->bdcomm = procel->world.split((status != IDLE) ? 1 : MPI_UNDEFINED);
                
    if(INVOLVED == status) 
    {
        procel->bdlocal  = procel->bdcomm.split(procel->mytype);
        procel->bdremote = procel->bdcomm - procel->bdlocal;
    }

    if(PROC_FE == procel->mytype) 
    {
        ChooseNodeOwners();
        AssignNodeIndices();
        
        /// Sort the nodes according to the value of Node::Index()
        listN.sort();
    }

    procel->world.barrier();
    Log::Instance().write("Finished setting up the handshake geometry.\n");
}

void HandshakeGeo::UpdateOnFE()
{
    fe::interface::Mesh *mesh = solver->GetMesh();

    for(Element el = mesh->ElementsBegin(); el != mesh->ElementsEnd(); ++el) 
        if(Intersects(el))
            listE.addElement(el);

    Log::Instance().write("Number of local handshake elements = %lu\n", listE.size());


    /// --------------------------------------------------------------------------------
    /// Write the handshake elements to a file
    fe::interface::Writer* w = fe::interface::Writer::New(solver);

    std::valarray<int> b(solver->GetMesh()->nelements());
    b = 0;

    /// TODO Since there is no element index we need to compute the intersections twice!
    int ii = 0;
    for(Element el = mesh->ElementsBegin(); el != mesh->ElementsEnd(); ++el, ++ii) 
        if(Intersects(el))
            b[ii] = 1;

    w->WriteElementArrayInt("handshake", solver, &b[0], "b");
    /// --------------------------------------------------------------------------------

    
    // Temporary array used to flag vectors
    fe::interface::NodalVector<int> t(solver->GetMesh());

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        t(nd) = 0;
    
    for(ElementList::ConstIterator it = elements().begin(); it != elements().end(); ++it)
        for(int c = 0; c < it->ncorners(); ++c) 
        {
            Node p = it->corner(c);
            if(0 == t(p))
            {
                t(p) = 1;
                listN.addNode(p);
            }
        }

    Log::Instance().write("Number of local handshake nodes = %lu\n", listN.size());
    
    status = (0 == listE.size()) ? IDLE : INVOLVED;
}

void HandshakeGeo::UpdateOnMD()
{
    int    i, k;
    double pos[3];

    system->AccessBegin(md::interface::System::RD_POS |
                        md::interface::System::WR_PIGGYBACK);   
    for(ParticleHandle ph = system->begin(); ph != system->end(); ++ph) 
    {
        for(k = 0; k < DIM; ++k)
            pos[k] = md::interface::Property::Pos(*system, ph, k);

        i = -1;
        if(IsInside(pos)) 
        {
            listP.AddParticle(ph);
            i = ph.GetLocalId();
        }
        // else: i stays -1

        md::interface::Property::Piggyback(*system, ph)->bdIndex = i;
    }
    system->AccessEnd();

    Log::Instance().write("Number of local handshake particles: %lu\n", listP.size());
    
    ParticleList list;
    list.AddSelection(SelectHandshakeParticles());
    CHECK_ALWAYS(list.size() == listP.size(), 0);

    status = (0 != listP.size()) ? INVOLVED : IDLE;
}

void HandshakeGeo::AssignNodeIndices()
{
    long idx = 0;
    fe::interface::Mesh*                mesh        = solver->GetMesh();
    fe::interface::NodeDistributionMap* distrib_map = solver->GetNodeDistribMap();

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        Node::Index(*solver, nd) = -1;

#ifndef NDEBUG
    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        CHECK_DEBUG(-1 == Node::Index(*solver, nd), 0);
#endif

    // Make sure that interior indices have a contiguous index range
    // starting at zero
    for(NodeList::Iterator it = listN.begin(); it != listN.end(); ++it)
        if(distrib_map->bOwn(*it))
            Node::Index(*solver, *it) = idx++;

    // Assign the indices to the rest. First the remaining nodes in the
    // handshake are treated (so that in consequence the nodes in the handshake
    // region are numbered contiguously) and then rest nodes come
    for(NodeList::Iterator it = listN.begin(); it != listN.end(); ++it)
        if(Node::Index(*solver, *it) < 0)
            Node::Index(*solver, *it) = idx++;
    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        if(Node::Index(*solver, nd) < 0)
            Node::Index(*solver, nd) = idx++;

    CHECK_ALWAYS(mesh->nnodes() == idx, 0);
}

void HandshakeGeo::ChooseNodeOwners()
{
    long N;

    fe::interface::Mesh*                mesh        = solver->GetMesh();
    fe::interface::NodeDistributionMap* distrib_map = solver->GetNodeDistribMap();

    // Priority vector
    fe::interface::NodalVector<int> P(mesh);

    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        P(nd)  = 0;
    for(NodeList::Iterator it = listN.begin(); it != listN.end(); ++it)
        P(*it) = 1;

    distrib_map->CreateMappingFromPriorities(*solver, procel->myscale.myrank(), P);

    N = 0;
    for(Node nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
        N += distrib_map->bOwn(nd);

    Log::Instance().write("Number of owned mesh nodes = %ld\n", N);

    N = 0;
    for(NodeList::Iterator it = listN.begin(); it != listN.end(); ++it)
        N += distrib_map->bOwn(*it);

    Log::Instance().write("Number of owned handshake nodes = %ld\n", N);
}

