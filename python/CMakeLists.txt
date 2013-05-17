
# We use SWIG to create the C++/Python interface
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE     (${SWIG_USE_FILE})

# Obviously we need to link against the python libaries
FIND_PACKAGE       (PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE(MACI_PROCESS_SWIG)
INCLUDE(MACI_CONFIG_PY)

# Make sure that the libraries are correctly prefixed
SET(CMAKE_SHARED_LIBRARY_PREFIX "_")
# Python doesn't like the .dylib suffix on OS X
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
ENDIF()


# ------------------------------------------------------------------------------
# Python modules
# ------------------------------------------------------------------------------
MACI_ADD_SUBDIRS(maci plugins)


# ------------------------------------------------------------------------------
# Maci executable
# ------------------------------------------------------------------------------
ADD_SUBDIRECTORY(exe)

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
FOREACH(DIM 2 3)
	FILE(GLOB FILES 
	     ${CMAKE_CURRENT_SOURCE_DIR}/*.py)
	INSTALL(FILES	   ${FILES}
		DESTINATION     lib/maci)

ENDFOREACH()

