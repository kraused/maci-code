
SET(MD_LIBRARY_DIRS "INVALID")
SET(FE_LIBRARY_DIRS "INVALID")

# Let MD_LIBRARY_DIRS point to the installation lib
# folder for the chosen MD toolkit.
IF(HAVE_TREMOLO)
        SET(MD_LIBRARY_DIRS ${Tremolo_LIBRARY_DIRS})
ENDIF()

IF(HAVE_MDGPGPU)
        SET(MD_LIBRARY_DIRS ${MDGPGPU_LIBRARY_DIRS})
ENDIF()

IF(HAVE_LAMMPS)
	SET(MD_LIBRARY_DIRS ${Lammps_LIBRARY_DIRS})
ENDIF()

# Let FE_LIBRARY_DIRS point to the installation lib
# directory of the FE toolkit
IF(HAVE_UG)
	SET(FE_LIBRARY_DIRS ${UG_LIBRARY_DIRS})
ENDIF(HAVE_UG)

# The variable MACI_DRIVER_FILE equals the name of
# the installed "driver" script, i.e., this is the
# script which initializes and finalizes everything
# and calls the user script internally
SET(MACI_DRIVER_FILE ${CMAKE_INSTALL_PREFIX}/lib/maci/Maci_Driver.py)

# The variable MACI_EXE equals the name of the 
# maci interpreter after installation
SET(MACI_EXE ${CMAKE_INSTALL_PREFIX}/bin/maci.exe)

# OS X is ignoring LD_LIBRARY_PATH ...
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(PLATFORM_LIBRARY_PATH "DYLD_LIBRARY_PATH")
ELSE()
	SET(PLATFORM_LIBRARY_PATH "LD_LIBRARY_PATH")
ENDIF()


# Configure all files
CONFIGURE_FILE(Shell.cc.in       ${CMAKE_CURRENT_BINARY_DIR}/Shell.cc)
CONFIGURE_FILE(maci.in           ${CMAKE_CURRENT_BINARY_DIR}/maci)
CONFIGURE_FILE(Maci_Driver.py.in ${CMAKE_CURRENT_BINARY_DIR}/Maci_Driver.py)

# Create the banner file
# In this file we collect information about the
# build time, svn status, ...
# It is run every time "make" is typed
ADD_CUSTOM_TARGET(banner ALL 
		  ${CMAKE_HOME_DIRECTORY}/cmake/create-banner.sh ${CMAKE_CURRENT_BINARY_DIR}/maci-banner.txt ${CMAKE_CURRENT_BINARY_DIR}/../../config.hh
      	          WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY})

# Build the driver application maci.exe
ADD_EXECUTABLE       (maci.exe ${CMAKE_CURRENT_BINARY_DIR}/Shell.cc)
TARGET_LINK_LIBRARIES(maci.exe ${PYTHON_LIBRARY} util)
ADD_DEPENDENCIES     (maci.exe banner)

# Installation
INSTALL(TARGETS			maci.exe
	RUNTIME DESTINATION	bin)
INSTALL(FILES			${CMAKE_CURRENT_BINARY_DIR}/maci
	DESTINATION		bin
	PERMISSIONS		OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
INSTALL(FILES			${CMAKE_CURRENT_BINARY_DIR}/Maci_Driver.py
	DESTINATION		lib/maci)
INSTALL(FILES                   ${CMAKE_CURRENT_BINARY_DIR}/maci-banner.txt
        DESTINATION             lib/maci)

