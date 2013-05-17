
// vi: tabstop=4:expandtab

#ifndef MACI_UMFPACK_MULTIPLIER_MATRIX_HH_INCLUDED
#define MACI_UMFPACK_MULTIPLIER_MATRIX_HH_INCLUDED 1

#include "MultiplierMatrix.hh"
#include "MultiplierAssembler.hh"
#include "StaticCommunicationGraph.hh"
#include "Int64.hh"
#include "DesignByContract.hh"
#include "mpi/Communicator.hh"

/// Forwarding
class Maci;


/// Base class for all implementations of the multiplier matrix by means of
/// a serial UMFPACK solver
class UmfpackMultiplierMatrix : public MultiplierMatrix
{

public:
    /// Create a new instance
    /// @param[in]  app the Maci application instance
    UmfpackMultiplierMatrix(Maci* app);

protected:
    /// Map the indices of the assembled (coordinate) matrix from the 
    /// scheme
    ///     +--------+----------+
    ///     | myrank | localIdx |
    ///     +--------+----------+
    /// to global indices
    void ModifyIndices(MultiplierAssembler *ass);

    /// Create the solver. The function takes care of mapping the local
    /// indices to the global ones
    void CreateSolver(MultiplierAssembler *ass);

};

#endif

