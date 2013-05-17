#
# Maci_TPL_Spblas.cmake: Spblas Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_Spblas "Enable Spblas" OFF)

MACI_TPL_REPORT(Spblas)
IF(NOT TPL_ENABLE_Spblas)
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - Spblas_INCLUDE_DIRS:PATH
# - Spblas_LIBRARY_DIRS:PATH
#
MACI_TPL_CHECK_INPUT(Spblas)

INCLUDE_DIRECTORIES(${Spblas_INCLUDE_DIRS})
LINK_DIRECTORIES   (${Spblas_LIBRARY_DIRS})

LIST(APPEND LIBS2 spblas)
LIST(APPEND LIBS3 spblas)

LIST(APPEND SRC2 "linalg/SparseBLASMatrix.cc")
LIST(APPEND SRC3 "linalg/SparseBLASMatrix.cc")

SET(HAVE_SPBLAS 1)

