
PROJECT(WMPI NONE)

# Everything below 2.6 might work but is not yet
# tested
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# MPI
# For now we assume that we have cc and c++/CC compiler
# wrappers available
# # We set the compiler to the mpicc and mpic++ wrappers.
# # This is the easiest way to ensure that all necessary
# # libraries are linked and that the include paths are
# # correct
# 
# FIND_PROGRAM(MPI_MPICC_EXE
#              mpicc
#              PATHS ${MPI_BINARY_DIRS}
#              DOC "MPI C compiler wrapper.")
# 
# IF(NOT MPI_MPICC_EXE)
#         MESSAGE(FATAL_ERROR "Could not find MPI C compiler wrapper.")
# ENDIF()
# 
# FIND_PROGRAM(MPI_MPICXX_EXE
#              mpic++ | NAMES mpiCC
#              PATHS ${MPI_BINARY_DIRS}
#              DOC "MPI C++ compiler wrapper.")
# 
# IF(NOT MPI_MPICXX_EXE)
#         MESSAGE(FATAL_ERROR "Could not find MPI C++ compiler wrapper.")
# ENDIF()
# 
# SET(CMAKE_C_COMPILER   ${MPI_MPICC_EXE} )
# SET(CMAKE_CXX_COMPILER ${MPI_MPICXX_EXE})


# Now we can enable the languages (since CMAKE_C_COMPILER and
# CMAKE_CXX_COMPILER are set)
ENABLE_LANGUAGE( C )
ENABLE_LANGUAGE( CXX )


# Python
FIND_PACKAGE(PythonLibs REQUIRED)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})


# Swig
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})


# Include the source directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})


# Make sure that the libraries are correctly prefixed
SET(CMAKE_SHARED_LIBRARY_PREFIX "")
# Python doesn't like the .dylib suffix on OS X
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
ENDIF()

ADD_CUSTOM_COMMAND(
	OUTPUT	wmpiW.cc
	COMMAND	${SWIG_EXECUTABLE}
	ARGS	-python -c++ -shadow ${include_flag} -o wmpiW.cc 
		${CMAKE_CURRENT_SOURCE_DIR}/wmpi.i
	DEPENDS	${CMAKE_CURRENT_SOURCE_DIR}/wmpi.i)

ADD_LIBRARY(_wmpi 
            wmpiW.cc
            communicator.cc
            wmpi.cc)

# Only necessary on OS X and breaks compilation on linux
# because cmake tries to link in -llibpythonX.Y
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	TARGET_LINK_LIBRARIES(_wmpi ${PYTHON_LIBRARY})
ENDIF()


# Installation
INSTALL(TARGETS 	_wmpi 
	DESTINATION	lib)
INSTALL(FILES		__init__.py
			communicator.py
			${CMAKE_CURRENT_BINARY_DIR}/wmpi.py
	DESTINATION	lib/mpi)

