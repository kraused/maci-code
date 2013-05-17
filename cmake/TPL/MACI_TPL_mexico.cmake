#
# Maci_TPL_mexico.cmake: mexico Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_mexico "Enable mexico" OFF)

MACI_TPL_REPORT(mexico)
IF(NOT TPL_ENABLE_mexico)
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - mexico_INCLUDE_DIRS:PATH
# - mexico_LIBRARY_DIRS:PATH
#
MACI_TPL_CHECK_INPUT(mexico)

INCLUDE_DIRECTORIES(${mexico_INCLUDE_DIRS})
LINK_DIRECTORIES   (${mexico_LIBRARY_DIRS})

LIST(APPEND LIBS2 mexico)
LIST(APPEND LIBS3 mexico)

SET(HAVE_mexico 1)

