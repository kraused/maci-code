
// vi: tabstop=4:expandtab

#ifndef MACI_PARALLEL_MULTIPLIER_MATRIX_HH_INCLUDED
#define MACI_PARALLEL_MULTIPLIER_MATRIX_HH_INCLUDED 1

#include "MultiplierMatrix.hh"
#include "Assembler.hh"
#include "Int64.hh"
#include "DesignByContract.hh"
#include "Logging.hh"

#include "mpi/Communicator.hh"
#include "utils/strprintf.hh"
#include "utils/parscan.hh"

/// Forwarding
class Maci;


/// Base class for all implementations of the multiplier matrix by means of
/// a parallel solver
///
/// Note that, since the lumped mass matrices have only entries at the
/// owned nodes, also the multiplier matrix has no entries for non-owned nodes
///
template<typename Solver>
class ParallelMultiplierMatrix : public MultiplierMatrix
{

public:
    /// Constructor
    /// @param[in]  app the Maci application instance
    ParallelMultiplierMatrix(Maci* app)
    : MultiplierMatrix(app)
    {
    }

protected:  
    /// Map the indices of the assembled (coordinate) matrix from the 
    /// scheme
    /// <code>
    /// +--------+----------+
    /// | myrank | localIdx |
    /// +--------+----------+
    ///</code>
    /// to global indices
    /// @param ass  the multiplier assembler
    void ModifyIndices(Assembler *ass)
    {
        int* starts;
        AssemblerOutputMatrix& A = this->GetPrimaryAssembler()->matrix;

        starts = new int[procel->world.size()];
        utils::par_exclusive_scan(procel->world, A.getRelativeNumOfRows(), starts);
        
        Log::Instance().write("Number of local rows = %ld\n", A.getRelativeNumOfRows());
        Log::Instance().write("starts = %s\n", utils::list2str("%d", starts, starts+procel->world.size()).c_str());

        for(std::size_t i = 0; i < A.size(); ++i) 
        {
            CHECK_DEBUG(     Int64::hi(A[i].row) == procel->world.myrank(), 0);
            CHECK_DEBUG(0 <= Int64::lo(A[i].row), 0);
            CHECK_DEBUG(0 <= Int64::hi(A[i].col) && Int64::hi(A[i].col) < procel->world.size(), 0);
            CHECK_DEBUG(0 <= Int64::lo(A[i].col), 0);

            A[i].row = starts[Int64::hi(A[i].row)] + Int64::lo(A[i].row);
            A[i].col = starts[Int64::hi(A[i].col)] + Int64::lo(A[i].col);
        }

        delete[] starts;
    }

    /// Create the solver. The function takes care of mapping the local
    /// indices to the global ones
    /// @param ass      the multiplier assembler
    /// @param MaxIter  maximal number of iterations
    /// @param Tol      tolerance to reach
    void CreateSolver(Assembler *ass, int MaxIter, float Tol)
    {
        ModifyIndices(ass);

        comm = procel->world.split((GetPrimaryAssembler()->matrix.size() > 0) ? 1 : MPI_UNDEFINED);
        if(comm.size() > 0)
            this->solver = new Solver(GetPrimaryAssembler(), comm, MaxIter, Tol);
    }

public:
    /// Communicator for the parallel solver
    mpi::Communicator comm;

};

#endif


