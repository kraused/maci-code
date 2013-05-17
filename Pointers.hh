
// vi: tabstop=4:expandtab

#ifndef MACI_POINTERS_HH_INCLUDED
#define MACI_POINTERS_HH_INCLUDED 1

#include "md/MolecularDynamics.hh"
#include "fe/FiniteElements.hh"

/// Forwarding
class Maci;
class ProcElement;
class HandshakeGeo;
class PluginTable;
class TimeIntegrator;
class Weights;
class Memory;


/// Pointers: Contains pointers to master copies of fundamental
///           classes in the libray
/// The design is stolen from LAMMPS (actually also parts of this
/// comment)
class Pointers
{

public:

    /// Constructor
    /// @param[in]  ptr pointer to the Maci application instance
    Pointers(Maci* ptr);

protected:
    /// Reference to the app instance
    Maci* app;

    /// Referenes to pointers are conceptually the
    /// same as double pointers but easier to handle

    /// Pointer to the FE Solver. The pointer is invalid
    /// on MD processing elements
    fe::interface::Solver*& solver;
    /// Pointer to the MD System. The pointer is invalid
    /// on FE processing elements
    md::interface::System*& system;

    /// Pointer to the ProcElement instance
    ProcElement*&           procel;
    /// The weight function for the energy. The values of the
    /// function differ on Molecular Dynamics and Finite
    /// Element processing elements
    Weights*                weights;

    /// Pointer to the Handshake instance
    HandshakeGeo*&          handshake;
    
    /// The plugin table
    PluginTable*&           plugins;

    /// Pointer to the time integrator
    TimeIntegrator*&        integrator;

    /// Pointer to the memory module
    Memory*                 memory;

//    /// Pointer to the transfer operator from MD to FE. This
//    /// is one part of the projection operator. The transfer operator
//    /// itself does not conserve units and must be multiplied by
//    /// inverse masses to obtain the correct projection operator
//    TransferOperatorBase*    transop;

};

#endif

