#
# Maci_TPL_wmpi.cmake: wmpi Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_wmpi "Enable wmpi" OFF)

MACI_TPL_REPORT(wmpi)
IF(NOT TPL_ENABLE_wmpi)
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - wmpi_INCLUDE_DIRS:PATH
# - wmpi_LIBRARY_DIRS:PATH
#
MACI_TPL_CHECK_INPUT(wmpi)

INCLUDE_DIRECTORIES(${wmpi_INCLUDE_DIRS})
LINK_DIRECTORIES   (${wmpi_LIBRARY_DIRS})

# No library to link against ...

SET(HAVE_wmpi 1)

