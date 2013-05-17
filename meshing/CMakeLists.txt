
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

# Create the include path
GET_DIRECTORY_PROPERTY(INCLUDE_DIRS INCLUDE_DIRECTORIES)
SET(INCLUDE_FLAG "")
FOREACH(VAR ${INCLUDE_DIRS})
        SET(INCLUDE_FLAG ${INCLUDE_FLAG} -I${VAR})
ENDFOREACH()

# Make sure that the libraries are correctly prefixed
SET(CMAKE_SHARED_LIBRARY_PREFIX "_")
# Python doesn't like the .dylib suffix on OS X
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
ENDIF()


FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

ADD_CUSTOM_COMMAND(
	OUTPUT	meshingW.cc
	COMMAND	${SWIG_EXECUTABLE}
	ARGS	-python -c++ -shadow -DDIM=${DIM} -DHAVE_CONFIG_HH=1 ${INCLUDE_FLAG} -o meshingW.cc 
		${CMAKE_CURRENT_SOURCE_DIR}/meshing.i
	DEPENDS	${CMAKE_CURRENT_SOURCE_DIR}/meshing.i)

ADD_LIBRARY(meshing SHARED meshingW ${SOURCES})
TARGET_LINK_LIBRARIES(meshing ${PYTHON_LIBRARY})

# ------------------------------------------------------------------------------
# meshgen executable
# ------------------------------------------------------------------------------
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/meshgen.in 
	       ${CMAKE_CURRENT_BINARY_DIR}/meshgen)

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
INSTALL(TARGETS         	meshing 
	LIBRARY DESTINATION	lib/maci)

FILE   (GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/*.py)
INSTALL(FILES           	${CMAKE_CURRENT_BINARY_DIR}/meshing.py
				${files}
        DESTINATION     	lib/maci/meshing)

INSTALL(FILES			${CMAKE_CURRENT_BINARY_DIR}/meshgen
	DESTINATION		bin
	PERMISSIONS		OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

