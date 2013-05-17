#
# Maci_TPL_Trilinos.cmake: Trilinos Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_Trilinos "Enable Trilinos" OFF)

MACI_TPL_REPORT(Trilinos)
IF(NOT TPL_ENABLE_Trilinos)
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - Trilinos_INCLUDE_DIRS:PATH
# - Trilinos_LIBRARY_DIRS:PATH
# - Trilinos_LIBRARY_NAMES:STRING
#
MACI_TPL_CHECK_INPUT(Trilinos)

INCLUDE_DIRECTORIES(${Trilinos_INCLUDE_DIRS})
LINK_DIRECTORIES   (${Trilinos_LIBRARY_DIRS})

LIST(APPEND SRC2 linalg/TrilinosSolver.cc)
LIST(APPEND SRC3 linalg/TrilinosSolver.cc)

LIST(APPEND LIBS2 ${Trilinos_LIBRARY_NAMES})
LIST(APPEND LIBS3 ${Trilinos_LIBRARY_NAMES})

SET(HAVE_TRILINOS 1)

