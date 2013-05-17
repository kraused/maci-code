#
# Maci_TPL_MDGPGPU.cmake: MDGPGPU Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_MDGPGPU "Enable MDGPGPU" OFF)

MACI_TPL_REPORT(MDGPGPU)
IF(NOT TPL_ENABLE_MDGPGPU)
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - MDGPGPU_INCLUDE_DIRS:PATH
# - MDGPGPU_LIBRARY_DIRS:PATH
#
MACI_TPL_CHECK_INPUT(MDGPGPU)

INCLUDE_DIRECTORIES(${MDGPGPU_INCLUDE_DIRS})
LINK_DIRECTORIES   (${MDGPGPU_LIBRARY_DIRS})

# TODO It might also be necessary to include some
#      CUDA directories, e.g. /opt/cuda/include on
#      the icstesla1 system

LIST(APPEND LIBS2 mdgpgpu)
LIST(APPEND LIBS3 mdgpgpu)

LIST(APPEND SRC2 md/mdgpgpu/ParticleHandle.cc
                 md/mdgpgpu/System.cc)
LIST(APPEND SRC3 md/mdgpgpu/ParticleHandle.cc
                 md/mdgpgpu/System.cc)

SET(HAVE_MDGPGPU 1)

