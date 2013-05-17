#
# Maci_TPL_fftw.cmake: fftw Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_fftw "Enable fftw" OFF)

MACI_TPL_REPORT(fftw)
IF(NOT TPL_ENABLE_fftw)
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - fftw_INCLUDE_DIRS:PATH
# - fftw_LIBRARY_DIRS:PATH
# - fftw_LIBRARY_NAMES:STRING
#
MACI_TPL_CHECK_INPUT(fftw)

INCLUDE_DIRECTORIES(${fftw_INCLUDE_DIRS})
LINK_DIRECTORIES   (${fftw_LIBRARY_DIRS})

# Necessary on OS X
LIST(APPEND LIBS2 ${fftw_LIBRARY_NAMES})
LIST(APPEND LIBS3 ${fftw_LIBRARY_NAMES})

SET(HAVE_FFTW 1)

