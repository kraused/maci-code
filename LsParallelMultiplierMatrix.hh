
// vi: tabstop=4:expandtab

#ifndef MACI_LS_PARALLEL_MULTIPLIER_MATRIX_HH_INCLUDED
#define MACI_LS_PARALLEL_MULTIPLIER_MATRIX_HH_INCLUDED 1

#include "ParallelMultiplierMatrix.hh"
#include "AssemblyProduct.hh"
#include "HandshakeGeo.hh"
#include "Function.hh"
#include "LsAssembler.hh"
#include "AtomWeightedMassAssembler.hh"
#include "LsLumpedMeshMassAssembler.hh"
#include "LumpedFEWeightedMassAssembler.hh"
#include "MultiplierAssembler.hh"
#include "DesignByContract.hh"
#include "Maci.hh"


/// Least squares multiplier matrix which is inverted using the parallel solver
template<typename Solver>
class LsParallelMultiplierMatrix : public ParallelMultiplierMatrix<Solver>
{

public:
    /// Create a new instance
    /// @param[in] app  The Maci application instance
    /// @param[in] MaxIter  maximal number of iterations
    /// @param[in] Tol  tolerance to reach
    LsParallelMultiplierMatrix(Maci* app, int MaxIter, double Tol)
    : ParallelMultiplierMatrix<Solver>(app)
    {
        ParticleList list;
        if(PROC_MD == gcpe()->mytype)
            list.AddSelection(SelectHandshakeParticles());

        /// Scale transfer operator
        AddSecondaryAssemblerIfNotExisting("LsTransfer", 
                                           new LsAssembler(*this->handshake));
        /// Atomistic mass matrix
        AddSecondaryAssemblerIfNotExisting("AtomWeightedMassMatrix",
                                           new AtomWeightedMassAssembler(list));
        /// Constraints matrix
        AddSecondaryAssemblerIfNotExisting("LsLumpedMeshMassMatrix", 
                                           new LsLumpedMeshMassAssembler(this->handshake, 
                                                                         this->GetSecondaryAssembler("LsTransfer")));
        /// Time integration matrix (weighted)
        AddSecondaryAssemblerIfNotExisting("LumpedFEWeightedMassMatrix",
                                           new LumpedFEWeightedMassAssembler(this->handshake));

        this->AddPrimaryAssembler("LsMultiplierMatrix", 
                                  new MultiplierAssembler(this->GetSecondaryAssembler("LsTransfer"),
                                                          this->GetSecondaryAssembler("AtomWeightedMassMatrix"),
                                                          this->GetSecondaryAssembler("LsLumpedMeshMassMatrix"),
                                                          this->GetSecondaryAssembler("LumpedFEWeightedMassMatrix")));
        this->CreateSolver(this->GetPrimaryAssembler(), MaxIter, Tol);
    }

};

#endif


