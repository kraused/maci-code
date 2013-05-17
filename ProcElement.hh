
// vi: tabstop=4:expandtab

#ifndef MACI_PROC_ELEMENT_H_INCLUDED
#define MACI_PROC_ELEMENT_H_INCLUDED 1

#include "Pointers.hh"
#include "likely.hh"

#include "mpi/Communicator.hh"
#include "mpi/Group.hh"
#include "exception/BadPlaceException.hh"

#include "polylib/polylib.hh"
#include "md/MolecularDynamics.hh"
#include "fe/FiniteElements.hh"


/// Forwarding
class Maci;


/// Type of processing elements 
typedef enum
{
    PROC_MD        = 0,   //< Indicates a molecular dynamics PE
    PROC_FE        = 1,   //< Indicates a finite element PE
    PROC_UNDEFINED = 2    //< Information not yet set?
} PeType;


/// The ProcElement is the central structure which contains all information
/// about this PE as, e.g., the communicators, the boundingbox, ...
class ProcElement : public Pointers
{
/// For SWIG
public:
    typedef md::interface::System   System;
    typedef fe::interface::Solver   Solver;

public:
    /// construct a new processing element
    /// @param[in]  type    either PROC_MD or PROC_FE
    /// @param[in]  app the Maci instance
    explicit ProcElement(PeType type, Maci* maci);

    // /// Destroy the processing element. Multiple invocation of constructor
    // /// and destructor results in undefined behavior. It works if the
    // /// underlying MD and FE implementations support this functionality
    // /// (e.g., UG does in theory but not in practice)
    // ~ProcElement();

    /// Returns the MD system or 0 if this PE is not a MD processing element
    inline System* GetMMSystem()
    {
        if(unlikely(PROC_MD != mytype))
            throw exception::BadPlaceException();

        CHECK_ALWAYS(NULL != system, "On MD processing elements system cannot be null!");
        return system;
    }

    /// Returns the MD system or 0 if this PE is not a MD processing element
    /// (const version)
    inline const System* GetMMSystem() const
    {
        if(unlikely(PROC_MD != mytype))
            throw exception::BadPlaceException();

        CHECK_ALWAYS(NULL != system, "On MD processing elements system cannot be null!");
        return system;
    }

    /// Return a pointer to the Finite Element Solver class which provides
    /// the interface to the FE code
    inline Solver* GetFESolver()
    {
        if(unlikely(PROC_FE != mytype))
            throw exception::BadPlaceException();

        CHECK_ALWAYS(NULL != solver, "On FE processing elements solver cannot be null!");
        return solver;
    }

    /// Return a pointer to the Finite Element Solver class which provides
    /// the interface to the FE code (const version)
    inline const Solver* GetFESolver() const
    {
        if(unlikely(PROC_FE != mytype))
            throw exception::BadPlaceException();

        CHECK_ALWAYS(NULL != solver, "On FE processing elements solver cannot be null!");
        return solver;
    }

    /// Get the characteristic length scale of the simulation
    inline double charLength() const
    {
        return M_charLength;
    }

    /// Get the absolute tolerance for this simulation
    inline double absTolerance() const
    {
        return M_absTolerance;
    }

public:
    /// Wrapper around MPI_COMM_WORLD
    mpi::Communicator world;
    /// Wrapper around the scale internal communicator of the md or fe
    /// side resp.
    mpi::Communicator myscale;

    /// Bridging domain communicator. The communicator is invalid on
    /// processes which do not touch the handshake region
    /// Initialized in HandshakeGeo::Update()
    mpi::Communicator bdcomm;
    /// Bridging domain communicator which only includes the processors
    /// of the same scale. The communicator is invalid on
    /// processes which do not touch the handshake region
    /// Initialized in HandshakeGeo::Update()
    mpi::Communicator bdlocal;
    /// Group of remote BD processes. The group is invalid on
    /// processes which do not touch the handshake region
    /// Initialized in HandshakeGeo::Update()
    mpi::Group bdremote; 
    
    /// Type of this PE
    PeType mytype;
    /// Bounding box of the PEs domain
    polylib::Box<double,DIM> boundbox;

private:
    /// The characteristic length scale of the system is used to define 
    /// in geometry related operations. It should be initialized once and 
    /// then kept constant throughout the simulation to ensure constistency
    double M_charLength;

    /// The absolute tolerance value. This depends on the precision of the
    /// mesh and MD floating point type
    double M_absTolerance;

};

/// Get a pointer to the current ProcElement on the processing element
ProcElement* gcpe();

#endif


