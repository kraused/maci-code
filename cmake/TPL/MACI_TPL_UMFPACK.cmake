#
# Maci_TPL_UMFPACK.cmake: UMFPACK Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_UMFPACK "Enable UMFPACK" OFF)

MACI_TPL_REPORT(UMFPACK)
IF(NOT TPL_ENABLE_UMFPACK)
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - UMFPACK_INCLUDE_DIRS:PATH
# - UMFPACK_LIBRARY_DIRS:PATH
# - UMFPACK_LIBRARY_NAMES:STRING
#
MACI_TPL_CHECK_INPUT(UMFPACK NAMES)

INCLUDE_DIRECTORIES(${UMFPACK_INCLUDE_DIRS})
LINK_DIRECTORIES   (${UMFPACK_LIBRARY_DIRS})

LIST(APPEND SRC2 L2UmfpackMultiplierMatrix.cc
                 LsUmfpackMultiplierMatrix.cc
                 UmfpackMultiplierMatrix.cc
                 linalg/SerialUmfpackSolver.cc)
LIST(APPEND SRC3 L2UmfpackMultiplierMatrix.cc
                 LsUmfpackMultiplierMatrix.cc
                 UmfpackMultiplierMatrix.cc
                 linalg/SerialUmfpackSolver.cc)

LIST(APPEND LIBS2 ${UMFPACK_LIBRARY_NAMES})
LIST(APPEND LIBS3 ${UMFPACK_LIBRARY_NAMES})

SET(HAVE_UMFPACK 1)

