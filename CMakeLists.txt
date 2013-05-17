
PROJECT(MACI NONE)

# Everything below 2.6 might work but is not yet
# tested
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
                      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/TPL)

INCLUDE(MACI_ADD_LIBRARY)
INCLUDE(MACI_ADD_SUBDIRS)

# Check for MPI. We need to cal this before we enable the
# languages because these files might set CMAKE_C_COMPILER
# and CMAKE_CXX_COMPILER to enforce usage of compiler
# wrappers
INCLUDE(MACI_TPL_MPI)

# Now we can enable the languages (since CMAKE_C_COMPILER and
# CMAKE_CXX_COMPILER are set)
ENABLE_LANGUAGE( C )
ENABLE_LANGUAGE( CXX )

# Installation
SET(LIB_INSTALL_DIR     ${CMAKE_INSTALL_PREFIX}/lib)
SET(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)
SET(BIN_INSTALL_DIR     ${CMAKE_INSTALL_PREFIX}/bin)
SET(MAN_INSTALL_DIR     ${CMAKE_INSTALL_PREFIX}/share/man)

# Sources to be linked into the maci2 and maci2 libaries
SET(SRC2 "")
SET(SRC3 "")

# Libraries to link into the maci2 and maci2 libraries
SET(LIBS2 "hash;pu;tree")
SET(LIBS3 "hash;pu;tree")

# Check for the various TLPs supported by the code
FOREACH(TPLNAME GA Spblas OSKI Trilinos PETSc UMFPACK TAU PVO UG Tremolo MDGPGPU Lammps wmpi mexico)
	INCLUDE(MACI_TPL_${TPLNAME})
ENDFOREACH()

# Other stuff
INCLUDE(MACI_BuildType)
INCLUDE(MACI_SysHeaders)

# Configuration file
CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/config.hh.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.hh)
ADD_DEFINITIONS(-DHAVE_CONFIG_HH=1)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR})

# Subdirectories: Include them first so that the targets are known
MACI_ADD_SUBDIRS(hash treelib pulib)

# The main MACI source files
SET (SRC "")
LIST(APPEND SRC
        AnnulusSector.cc
        AssemblyProduct.cc
        Cuboid.cc
        ElementList.cc
        ElementShadow.cc
        ElementShadowList.cc
        FineFluctuationFilterAssembler.cc
        FineFluctuationFilter.cc
		FineFluctuationFilterJob.cc
        HandshakeGeo.cc
        PatchyHandshakeGeo.cc
        InterpolationAssembler.cc
        InterpolationFunction.cc
        L2Assembler.cc
        L2FineFluctuationFilter.cc
        L2Projection.cc
        L2Transfer.cc
        ResidualNormComputer.cc
		ResidualNormComputerJob.cc
        LsAssembler.cc
        LsFineFluctuationFilter.cc
        LsProjection.cc
        LsTransfer.cc
        MetaAssembler.cc
        MetaAssemblerImplementation.cc
        MultiplierAssembler.cc
        MultiplierMatrix.cc
        NodeList.cc
        CoordExchange.cc
        ParticleList.cc
        ParticleListSelector.cc
        PatchyFunction.cc
        DampingMethod.cc
        NoopDamper.cc
        DampingCoefficientsComputer.cc
        FrictionDamper.cc
        PMLDamper.cc
        PMLFineFluctuationDamper.cc
        FrictionFineFluctuationDamper.cc
        ProcElement.cc
        ProjectionAssembler.cc
        ProjectionBase.cc
		ProjectionBaseJob.cc
        DAEqCorrector.cc
        RattleCorrector.cc
		RattleCorrectorJob.cc
		RattleCorrectorCorrectionAssembler.cc
		RattleCorrectorTruncatedLumpedCorrectionMatrix.cc
		RattleCorrectorLsLumpedCorrectionMatrix.cc
        TransferOperator.cc
        TreeAssembler.cc
        TreeAssemblerIntermediateFormat.cc
        Weights.cc
        TimeIntegrator.cc
        RattleIntegrator.cc
        VerletIntegrator.cc
        Callback.cc
        TauProfiling.cc
        Logging.cc
        Memory.cc
        Maci.cc
        Pointers.cc
		MexicoUser.cc
		ParallelMatMul.cc
		ModifyIndicesAndUpdatePiggybackData.cc
		AtomMassAssembler.cc
		AtomWeightedMassAssembler.cc
		LsLumpedMeshMassMatrix.cc
		LsLumpedMeshMassAssembler.cc
		MeshMassAssembler.cc
		LumpedMeshMassAssembler.cc
		LumpedFEWeightedMassAssembler.cc
		LumpedFEWeightedMassMatrix.cc
		MeshMassMatrix.cc
		TruncatedLumpedMeshMassMatrix.cc
		TruncatedLumpedMeshMassAssembler.cc
        linalg/CSRHelper.cc
        linalg/CoordMatrix.cc
        linalg/Solver.cc
        mpi/Cartcommunicator.cc
        mpi/Communicator.cc
        mpi/Group.cc
        mpi/Intercommunicator.cc
        mpi/Irequest.cc
        exception/BadArgException.cc
        exception/BadCallException.cc
        exception/BadCheckException.cc
        exception/BadDefaultException.cc
        exception/BadPlaceException.cc
        exception/BadPostcondException.cc
        exception/BadPrecondException.cc
        exception/ExceptionBase.cc
        exception/Failure.cc
        Plugin.cc
        PluginTable.cc
        plugins/Visualize.cc
        plugins/MonitorResidual.cc
        plugins/MonitorDampingForce.cc
        plugins/MeasureEnergy.cc
        plugins/ShowProgress.cc
        utils/strprintf.cc
		utils/parscan.cc
        md/Writer.cc
        fe/Writer.cc)

# CRS Matrix is the default if no TPL is there which
# can do the job
IF(NOT HAVE_MKL AND NOT HAVE_OSKI AND NOT HAVE_SPBLAS)
	LIST(APPEND SRC
	            linalg/SparseCSRMatrix.cc)
ENDIF()

LIST(APPEND SRC2 "${SRC}")
LIST(APPEND SRC3 "${SRC}")

# The maci library contains most of the C++ files in the
# MACI package
# TODO This can be a static library but then we need to add -fPIC
#      to the compiler flags
MACI_ADD_LIBRARY(maci SHARED "${SRC2}" "${SRC3}" "${LIBS2}" "${LIBS3}")

# Further subdirectories
MACI_ADD_SUBDIRS(python meshing scripts)

# Other files to configure and install
# commlib namelists: The configuration files for commlib. These are
#					 configured by CMake to ensure that we do not end
#					 up with invalid configurations (e.g., trying to
#	                 use GA runtime if the GA library is not installed)
FOREACH(C RattleCorrector ResidualNormComputer FineFluctuationFilter ProjectionBase)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${C}.nml.in
			       ${CMAKE_CURRENT_BINARY_DIR}/${C}.nml)
	INSTALL(FILES  ${CMAKE_CURRENT_BINARY_DIR}/${C}.nml
			DESTINATION share/maci)
ENDFOREACH()

