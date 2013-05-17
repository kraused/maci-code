//
// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#if 2 == DIM
%module maciS2
#else
%module maciS3
#endif
%{

#include "config.hh"
#include "Maci.hh"

/* If we do not include the headers for plugins
   here, the SWIG generated wrapper file will not
   compile */
#include "plugins/Visualize.hh"
#include "plugins/MonitorResidual.hh"
#include "plugins/MonitorDampingForce.hh"
#include "plugins/MeasureEnergy.hh"
#include "plugins/ShowProgress.hh"

%}

%include "valarray.i"

%include "config.hh"

#ifdef HAVE_UG
%import  "python/ug.i"
%include "ug/SolverImplementation.i"
%include "ug/NeumannForceCalculator.i"
%include "ug/DirichletForceCalculator.i"
%include "ug/DirichletEnforcer.i"
%include "ug/ugTimeIntegrator.i"
%include "ug/ugVerletIntegrator.i"
%include "ug/ugRattleIntegrator.i"
#endif

#ifdef HAVE_TREMOLO
%include "tremolo/tremoloInstance.i"
%include "tremolo/tremoloTimeIntegrator.i"
%include "tremolo/tremoloVerletIntegrator.i"
%include "tremolo/tremoloRattleIntegrator.i"
#endif

#ifdef HAVE_LAMMPS
%include "lammps/lammpsInstance.i"
%include "lammps/lammpsComputeList.i"
#endif

%include "Maci.hh"
/* Extensions to the Maci class. These are placed here
   because we cannot create a Maci.i file (By default the
   filesystem on OS X is case insensitive)
 */
%extend Maci
{

    void SetSolver(void* impl)
    {
        self->solver = new fe::interface::Solver(*(fe::interface::Solver::Implementation* )impl);

        double lower[3], upper[3];
        self->solver->GetMesh()->BoundingBox(lower, upper);
    }

    void SetSystem(void* impl)
    {
        self->system = new md::interface::System(*(md::interface::System::Implementation* )impl);
    }

}

/// Only to get rid of some SWIG warnings:
%include "MexicoUser.i"

%include "Pointers.i"
%include "AssemblyProduct.i"
%include "ProcElement.i"
%include "Logging.i"
%include "HandshakeGeo.i"
%include "GeoPrimitive.i"
%include "PatchyHandshakeGeo.i"
%include "Cuboid.i"
%include "AnnulusSector.i"
%include "Function.i"
%include "ConstantFunction.i"
%include "InterpolationFunction.i"
%include "PatchyFunction.i"
%include "VectorFunction.i"
%include "Weights.i"
%include "InterpolationWeights.i"
%include "DAEqCorrector.i"
%include "RattleCorrector.i"
%include "ParticleList.i"
%include "PUSpace.i"
%include "TransferOperator.i"
%include "ProjectionBase.i"
%include "L2Transfer.i"
%include "LsTransfer.i"
%include "L2Projection.i"
%include "LsProjection.i"
%include "MultiplierMatrix.i"
%include "ParallelMultiplierMatrix.i"
%include "L2ParallelMultiplierMatrix.i"
%include "LsParallelMultiplierMatrix.i"
#ifdef HAVE_UMFPACK
%include "UmfpackMultiplierMatrix.i"
%include "L2UmfpackMultiplierMatrix.i"
%include "LsUmfpackMultiplierMatrix.i"
#endif
#ifdef HAVE_TRILINOS
%include "ParallelMultiplierMatrix.i"
%include "L2TrilinosMultiplierMatrix.i"
%include "LsTrilinosMultiplierMatrix.i"
#endif
#ifdef HAVE_PETSC
%include "L2PetscMultiplierMatrix.i"
%include "LsPetscMultiplierMatrix.i"
#endif
%include "MeshMassMatrix.i"
%include "TruncatedLumpedMeshMassMatrix.i"
%include "LsLumpedMeshMassMatrix.i"
%include "RattleCorrectorTruncatedLumpedCorrectionMatrix.i"
%include "RattleCorrectorLsLumpedCorrectionMatrix.i"
%include "FineFluctuationFilter.i"
%include "L2FineFluctuationFilter.i"
%include "LsFineFluctuationFilter.i"
%include "DampingMethod.i"
%include "NoopDamper.i"
%include "FrictionDamper.i"
%include "FrictionFineFluctuationDamper.i"
%include "PMLDamper.i"
%include "PMLFineFluctuationDamper.i"
%include "TimeIntegrator.i"
%include "RattleIntegrator.i"
%include "VerletIntegrator.i"

/* Addressing relative to the maci root directory */
%import  "python/plugins/plugins.i"
%include "PluginTable.i"

