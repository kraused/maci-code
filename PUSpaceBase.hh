
// vi: tabstop=4:expandtab

#ifndef MACI_PUSPACE_BASE_HH_INCLUDED
#define MACI_PUSPACE_BASE_HH_INCLUDED 1

#include "ProcElement.hh"
#include "HandshakeGeo.hh"
#include "ParticleList.hh"
#include "mpi/Cartcommunicator.hh"

#include "pulib/pulib.hh"
#include "polylib/polylib.hh"

#include "md/MolecularDynamics.hh"

/// Base class for the Partition of unity space. This class allows us to compute
/// all the quantities need by the constructor of the pulib approximation space.
class PUSpaceBase
{
    typedef pulib::Types<DIM>::Box    Box;
    typedef pulib::Types<DIM>::double3 PulibDouble3;

public:
    /// Create an approximation space using the particles in the handshake
    /// region
    /// @param hs   the handshake geometry
    /// @param h    the patch size (same for all particles)
    PUSpaceBase(const HandshakeGeo& hs, const float h[])
    : communicator(0), N(0), pts(0), hdiams(0), ids(0), box(0)
    {
        memset(nbranks, 0x0, sizeof(nbranks));

        // If we have a handshake geometry then the bd indices have been
        // assigned to the particles
        if(PROC_MD == gcpe()->mytype) 
        {
            ParticleList list;
            list.AddSelection(SelectHandshakeParticles());

            CHECK_ALWAYS(hs.particles().size() == list.size(), 0);

            initializeP(list,h);
        } 
        else 
        {
            initializeM(h);
        }

        CHECK_POSTCOND_ALWAYS(communicator);
        CHECK_POSTCOND_ALWAYS(box);
    }

    /// Create an approximation space from particle list
    /// @param list the particle list
    /// @param h    the patch size (same for all particles)
    PUSpaceBase(const ParticleList& list, const float h[])
    : communicator(0), N(0), pts(0), hdiams(0), ids(0), box(0)
    {
        if(PROC_MD == gcpe()->mytype) 
        {
            initializeP(list,h);
        } else 
        {
            initializeM(h);
        }
    }

protected:
    /// The partition of unity space communicator. This one is 0 on mesh
    /// processing elements
    mpi::Communicator *communicator;
    /// Neighbour ranks
    int nbranks[pulib::ApproximationSpace<DIM>::NBNEIGHBORS+1];
    /// Number of points
    int N;
    /// Array of points
    PulibDouble3 *pts;
    /// Array of patch sizes
    PulibDouble3 *hdiams;
    /// Particle ids
    int *ids;
    /// bounding box
    Box *box;

    /// Initialization function (particle processors)
    /// @param list particle list
    /// @pararm h   the patch size (same for all particles)
    void initializeP(const ParticleList& list, const float h[])
    {
        CHECK_PRECOND_ALWAYS(PROC_MD == gcpe()->mytype);
        CHECK_PRECOND_ALWAYS(0 == list.size() || (0 != gcpe()->bdcomm.size()));

        if(0 == list.size()) {
            communicator = new mpi::Communicator(MPI_COMM_SELF);
        } else {
            communicator = new mpi::Communicator(gcpe()->bdlocal);
        }
        box = getBoundingBox(gcpe()->GetMMSystem());

        // Temporary cartesian communicator to get the neighbour
        // ranks
        mpi::Cartcommunicator *cart = 
            NonperCartesianCommunicator(gcpe()->myscale.getHandle());

        if(0 < list.size()) {
            loopAndFillArrays(list,gcpe()->GetMMSystem(),h);
            computeNeighbourRanks(*cart);
        }

        delete cart;

        communicator->barrier();
    }

    /// Initialization function (mesh processors)
    /// @pararm h   the patch size (same for all particles)
    void initializeM(const float h[])
    {
        CHECK_PRECOND_ALWAYS(PROC_FE == gcpe()->mytype);

        communicator = new mpi::Communicator(MPI_COMM_SELF);
        box = new Box;
    }

    /// Create a non-periodic cartesian communicator from a cartesian one
    /// @param comm     a cartesian communicator
    /// @returns        a new cartesian communicator which is not
    ///         periodic but otherwise similar to the input
    ///         communicator
    mpi::Cartcommunicator *NonperCartesianCommunicator(MPI_Comm comm) const
    {
        int status, dims[3], periods[3], coords[3];

        MPI_Topo_test(comm, &status);
        CHECK_PRECOND_ALWAYS(MPI_CART == status);

        MPI_Cart_get(comm, 3, dims, periods, coords);

        periods[0] = 0;
        periods[1] = 0;
        periods[2] = 0;

        // Do not reorder ranks
        return new mpi::Cartcommunicator(comm, DIM, dims, periods, 0);
    }

    /// Get the bounding box of particle processors
    Box *getBoundingBox(md::interface::System *sys)
    {
        double lower[3], upper[3];

        CHECK_PRECOND_ALWAYS(sys);
        sys->GetSpatialRegion(lower, upper);

        return new Box(lower, upper);
    }

    /// Loop over atoms and fill the particle and patch arrays
    /// @param list the particle list
    /// @param sys  the atomistic system
    /// @param h    the patch sizes
    void loopAndFillArrays(const ParticleList& list, md::interface::System *sys, const float h[])
    {
        using md::interface::Property;
        using md::interface::System;

        CHECK_PRECOND_ALWAYS(sys);
        
        int ii, t, j;

        if(0 == (N = list.size())) 
            return;

        pts    = new PulibDouble3[N];
        hdiams = new PulibDouble3[N];
        ids    = new int[N];

        // We need to map the local indices to global indices by adding
        // a "bottom" value
        ii = 0;
        // Use the number of local atoms instead of the list length because
        // the local indices of the list atoms may be scattered ...
        t  = sys->NlocalAtoms();
        communicator->exscan(&t, &ii, 1, MPI_INT, MPI_SUM);

        sys->AccessBegin(md::interface::System::RD_POS);
        
        j = 0;
        for(ParticleList::ConstIterator i  = list.begin(); i != list.end(); ++i, ++j) 
        {
            ids[j] = i->GetLocalId() + ii;

            for(int d = 0; d < DIM; ++d) 
            {
                pts[j][d]    = Property::Pos(*sys,*i,d);
                hdiams[j][d] = h[d];

#ifndef NDEBUG
                double x = 1e-5*(box->getUpper()[d] - 
                         box->getLower()[d]);
#endif
                CHECK_DEBUG(box, 0);
                CHECK_DEBUG(pts[j][d] > (box->getLower()[d] - x) &&
                        pts[j][d] < (box->getUpper()[d] + x), 0);
            }
        }

        sys->AccessEnd();
    }

    /// Compute the ranks of neighbour. Since only HS processors are relevant
    /// we need to modify the ranks of the cartesian communicator
    void computeNeighbourRanks(mpi::Cartcommunicator& cart)
    {
        CHECK_PRECOND_ALWAYS(communicator);
        CHECK_PRECOND_ALWAYS(PROC_MD == gcpe()->mytype);

        int n = 0;

        for(int i = -1; i <= 1; ++i) {
            for(int j = -1; j <= 1; ++j) {
                if(2 == DIM) {
                    nbranks[n++] = mpi::Group::translate(cart,
                                cart.neighbour(i,j, 0),
                                *communicator);
                } else {
                    nbranks[n++] = mpi::Group::translate(cart,
                                cart.neighbour(i,j,-1),
                                *communicator);
                    nbranks[n++] = mpi::Group::translate(cart,
                                cart.neighbour(i,j, 0),
                                *communicator);
                    nbranks[n++] = mpi::Group::translate(cart,
                                cart.neighbour(i,j,+1),
                                *communicator);
                }
            }
        }
    }

public:
    /// Convert global to local indices. This is necessary as we need globally
    /// unique indices
    int globalToLocal(int gid) const
    {
        int lid = 0;
        while(lid < N and gid != ids[lid]) { ++lid; }

        CHECK_POSTCOND_ALWAYS(lid < N);
        CHECK_POSTCOND_DEBUG (gid == ids[lid]);
        return lid;
    }

};

#endif

