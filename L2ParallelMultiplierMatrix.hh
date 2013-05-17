
// vi: tabstop=4:expandtab

#ifndef MACI_L2_PARALLEL_MULTIPLIER_MATRIX_HH_INCLUDED
#define MACI_L2_PARALLEL_MULTIPLIER_MATRIX_HH_INCLUDED 1

#include "ParallelMultiplierMatrix.hh"
#include "AssemblyProduct.hh"
#include "HandshakeGeo.hh"
#include "Function.hh"
#include "L2Assembler.hh"
#include "AtomWeightedMassAssembler.hh"
#include "TruncatedLumpedMeshMassAssembler.hh"
#include "LumpedFEWeightedMassAssembler.hh"
#include "MultiplierAssembler.hh"
#include "DesignByContract.hh"
#include "Maci.hh"
#include "ConstantFunction.hh"


/// L2 Multiplier matrix which is inverted using the parallel solver
template<typename Solver>
class L2ParallelMultiplierMatrix : public ParallelMultiplierMatrix<Solver>
{

public:
    /// Create a new instance
    /// @param[in] app  The Maci application instance
    /// @param[in] h    patch sizes (same for all particles)
    /// @param[in] MaxIter  maximal number of iterations
    /// @param[in] Tol  tolerance to reach
    L2ParallelMultiplierMatrix(Maci* app, const float h[], int MaxIter, double Tol)
    : ParallelMultiplierMatrix<Solver>(app)
    {
        ConstantFunction<double, 3> ones;

        ParticleList list;
        if(PROC_MD == gcpe()->mytype)
            list.AddSelection(SelectHandshakeParticles());

        /// Scale transfer operator
        AddSecondaryAssemblerIfNotExisting(L2UniqueAssemblerName("L2Transfer", h),
                                           new L2Assembler(*this->handshake, h));
        /// Atomistic mass matrix
        AddSecondaryAssemblerIfNotExisting("AtomWeightedMassMatrix",
                                           new AtomWeightedMassAssembler(list));
        /// Constraints matrix
        AddSecondaryAssemblerIfNotExisting("TruncatedLumpedMeshMassMatrix",
                                           new TruncatedLumpedMeshMassAssembler(this->handshake, &ones));
        /// Time integration matrix (weighted)
        AddSecondaryAssemblerIfNotExisting("LumpedFEWeightedMassMatrix",
                                           new LumpedFEWeightedMassAssembler(this->handshake));

        this->AddPrimaryAssembler(L2UniqueAssemblerName("L2MultiplierMatrix", h),
                                  new MultiplierAssembler(this->GetSecondaryAssembler(L2UniqueAssemblerName("L2Transfer", h)),
                                                          this->GetSecondaryAssembler("AtomWeightedMassMatrix"),
                                                          this->GetSecondaryAssembler("TruncatedLumpedMeshMassMatrix"),
                                                          this->GetSecondaryAssembler("LumpedFEWeightedMassMatrix")));
        this->CreateSolver(this->GetPrimaryAssembler(), MaxIter, Tol);
    }

};

#endif

