#
# Maci_TPL_[T].cmake: [T] Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_[T] "Enable [T]" OFF)

MACI_TPL_REPORT([T])
IF(NOT TPL_ENABLE_[T])
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - [T]_INCLUDE_DIRS:PATH
# - [T]_LIBRARY_DIRS:PATH
#
MACI_TPL_CHECK_INPUT([T])

INCLUDE_DIRECTORIES(${[T]_INCLUDE_DIRS})
LINK_DIRECTORIES   (${[T]_LIBRARY_DIRS})

# Do the magic here ...

SET(HAVE_[T] 1)

