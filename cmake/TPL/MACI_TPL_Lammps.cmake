#
# Maci_TPL_Lammps.cmake: Lammps Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_Lammps "Enable Lammps" OFF)

MACI_TPL_REPORT(Lammps)
IF(NOT TPL_ENABLE_Lammps)
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - Lammps_INCLUDE_DIRS:PATH
# - Lammps_LIBRARY_DIRS:PATH
# - Lammps_LIBRARY_NAMES:STRING
#
MACI_TPL_CHECK_INPUT(Lammps)

INCLUDE_DIRECTORIES(${Lammps_INCLUDE_DIRS})
LINK_DIRECTORIES   (${Lammps_LIBRARY_DIRS})

LIST(APPEND LIBS2 "${Lammps_LIBRARY_NAMES}")
LIST(APPEND LIBS3 "${Lammps_LIBRARY_NAMES}")

LIST(APPEND SRC2 md/lammps/ParticleHandle.cc
                 md/lammps/System.cc
                 md/lammps/src/CallbackCompilationUnit.cc
                 md/lammps/src/lammpsPiggyback.cc
                 md/lammps/src/lammpsInstance.cc
                 md/lammps/src/lammpsAtomVecMultiscaleAtomic.cc
                 md/lammps/src/lammpsTimeIntegrator.cc
                 md/lammps/src/lammpsRattleIntegrator.cc
                 md/lammps/src/lammpsVerletIntegrator.cc
                 md/lammps/src/lammpsNoOpIntegrator.cc
                 md/lammps/src/lammpsPairLJCutWeighted.cc
                 md/lammps/src/lammpsComputeList.cc
                 md/lammps/src/lammpsComputePotentialEnergyPerAtom.cc
                 md/lammps/src/lammpsComputeKineticEnergyPerAtom.cc
                 md/lammps/src/lammpsComputeStressPerAtom.cc)
LIST(APPEND SRC3 md/lammps/ParticleHandle.cc
                 md/lammps/System.cc
                 md/lammps/src/CallbackCompilationUnit.cc
                 md/lammps/src/lammpsPiggyback.cc
                 md/lammps/src/lammpsInstance.cc
                 md/lammps/src/lammpsAtomVecMultiscaleAtomic.cc
                 md/lammps/src/lammpsTimeIntegrator.cc
                 md/lammps/src/lammpsRattleIntegrator.cc
                 md/lammps/src/lammpsVerletIntegrator.cc
                 md/lammps/src/lammpsNoOpIntegrator.cc
                 md/lammps/src/lammpsPairLJCutWeighted.cc
                 md/lammps/src/lammpsComputeList.cc
                 md/lammps/src/lammpsComputePotentialEnergyPerAtom.cc
                 md/lammps/src/lammpsComputeKineticEnergyPerAtom.cc
                 md/lammps/src/lammpsComputeStressPerAtom.cc)

SET(HAVE_LAMMPS 1)

# Lammps depends on FFTW so we include the TPL file here
INCLUDE(MACI_TPL_fftw)

