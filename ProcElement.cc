
// vi: tabstop=4:expandtab

#include "ProcElement.hh"
#include "DesignByContract.hh"
#include "Callback.hh"
#include "Logging.hh"
#include "Maci.hh"

/* -----------------------------------------------------------------------------
 * Static, global variables
 * -------------------------------------------------------------------------- */

static ProcElement* gprocel;

/* -----------------------------------------------------------------------------
 * Static and inline functions
 * -------------------------------------------------------------------------- */
ProcElement* gcpe()
{
    return gprocel;
}

/// Compute the bounding box on FE processing elements
static polylib::Box<double,DIM> ComputeFEBoundingbox(const fe::interface::Solver* solver)
{
    double lower[3], upper[3];
    solver->GetMesh()->BoundingBox(lower, upper);

    return polylib::Box<double,DIM>(lower,upper);
}

/// Compute the bounding box on MD processing elements
static polylib::Box<double,DIM> ComputeMDBoundingbox(const md::interface::System* sys)
{
    double lower[3], upper[3];

    if(md::interface::System::UsesSpatialDecomp) 
        sys->GetSpatialRegion(lower, upper);
    else
    {
        THROW(Failure, "Don't know how to handle a code not using "
                       "a spatial decomposition.");
    }

    return polylib::Box<double,DIM>(lower,upper);
}

/// Clear the MD piggyback data
static void ClearPiggyback(md::interface::System* sys)
{
    using namespace md::interface;

    sys->AccessBegin(System::WR_PIGGYBACK);
    for(ParticleHandle ph = sys->begin(); ph != sys->end(); ++ph) 
    {
        PiggybackType *t = Property::Piggyback(*sys,ph);
        
        t->bdIndex  = -1;
        t->origin   = -1;
        t->locIndex = -1;

        std::fill(t->feprocs, t->feprocs + PiggybackType::MAX_FEPROCS, MPI_PROC_NULL);
        std::fill(t->fedispl, t->fedispl + PiggybackType::MAX_FEPROCS, 0);

        std::fill(t->bdprocs, t->bdprocs + PiggybackType::MAX_BDPROCS, MPI_PROC_NULL);
        std::fill(t->bddispl, t->bddispl + PiggybackType::MAX_BDPROCS, 0);
    }   
    sys->AccessEnd();
}

/* -----------------------------------------------------------------------------
 * Class member functions
 * -------------------------------------------------------------------------- */
ProcElement::ProcElement(PeType type, Maci* app)
: Pointers(app), world(MPI_COMM_WORLD), mytype(type)
{
    CHECK_PRECOND_ALWAYS(PROC_MD == type || PROC_FE == type);   
    myscale = world.split(type);

    gprocel = this;

    // Install the callbacks used by the MD and FE toolkits. See Callback.hh
    // for an explanation why we use callbacks
    InstallCallbacks();

    switch(mytype) 
    {
    case PROC_FE:
        Log::Instance().write("This is FE processing element %2d of %2d\n", myscale.myrank(), myscale.size());

        CHECK_ALWAYS(solver->GetMesh()->nnodes() > 0, "Wrong loadbalancing?");

        boundbox = ComputeFEBoundingbox(solver);

        // Compute the characteristic length scale as the maximal diameter
        // of a finite element
        M_charLength   = solver->GetMesh()->MaxElementDiameter();

        // We assume that the finite element part is double precision
        // which is not a strong assumption
        M_absTolerance = 1e-7;

        break;      
    case PROC_MD:
        Log::Instance().write("This is MD processing element %2d of %2d\n", myscale.myrank(), myscale.size());
        
        boundbox = ComputeMDBoundingbox(system);
        myscale  = mpi::Communicator(GetMMSystem()->GetCommunicator());
    
        ClearPiggyback(system);
        
        // We work typically in normalized units
        M_charLength   = 1.0;

        // Change tolerance 
        M_absTolerance = (4 == sizeof(System::fp_t)) ? 1e-5 : 1e-7;

        break;
    default:
        THROW(BadDefaultException,"");
    }
    
    world.allreduce(MPI_IN_PLACE, &M_charLength  , 1, MPI_DOUBLE, MPI_MAX);
    world.allreduce(MPI_IN_PLACE, &M_absTolerance, 1, MPI_DOUBLE, MPI_MAX);

    Log::Instance().write("Characteristic length = %e.\n", M_charLength);

    CHECK_POSTCOND_ALWAYS(solver || system);
}

