
// vi: tabstop=4:expandtab

#ifndef MACI_MACI_HH_INCLUDED
#define MACI_MACI_HH_INCLUDED 1

#include <md/MolecularDynamics.hh>
#include <fe/FiniteElements.hh>

/// Forwarding
class ProcElement;
class HandshakeGeo;
class PluginTable;
class TimeIntegrator;
class Weights;
class Memory;


/// Maci: The main application
/// In the Maci class (which usually has only a single instance)
/// we collect the different instances/classes needed to perform
/// simulations
class Maci
{

public:
    /// Create a new 
    Maci();

    /// Deletes all member instances
    ~Maci();

    /// Pointer to the FE Solver. The pointer is invalid
    /// on MD processing elements
    fe::interface::Solver*  solver;
    /// Pointer to the MD System. The pointer is invalid
    /// on FE processing elements
    md::interface::System*  system;

    /// Pointer to the ProcElement instance
    ProcElement*            procel;

    /// Pointer to the Handshake instance
    HandshakeGeo*           handshake;
    /// The weight function for the energy. The values of the
    /// function differ on Molecular Dynamics and Finite
    /// Element processing elements
    Weights*                weights;

    /// The plugin table
    PluginTable*            plugins;

    /// Pointer to the time integrator
    TimeIntegrator*         integrator;

    /// TODO We cannot simply treat the log class in the same
    ///      way as we do, e.g., treat plugins because it is
    ///      essential that we can access the logfile from each
    ///      place, whether the containing class is derived from
    ///      Pointers or not.
//    /// Pointer to the log instance
//    Log*                    log;

    /// Pointer to the memory module
    Memory*                 memory;
};

#endif

