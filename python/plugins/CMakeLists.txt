
# ------------------------------------------------------------------------------
# Plugins
# ------------------------------------------------------------------------------
MACI_PROCESS_SWIG(pluginsW2.cc ${CMAKE_CURRENT_SOURCE_DIR}/plugins.i DIM 2)
MACI_PROCESS_SWIG(pluginsW3.cc ${CMAKE_CURRENT_SOURCE_DIR}/plugins.i DIM 3)

MACI_ADD_LIBRARY(pluginsS SHARED "pluginsW2.cc" "pluginsW3.cc" "maci2;${LIBS2};${PYTHON_LIBRARY}" "maci3;${LIBS3};${PYTHON_LIBRARY}" "-Wno-uninitialized")

# Install Python files
INSTALL(FILES          ${CMAKE_CURRENT_BINARY_DIR}/pluginsS2.py
                       ${CMAKE_CURRENT_BINARY_DIR}/pluginsS3.py
        DESTINATION    lib/maci)


# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
FOREACH(DIM 2 3)
	SET (FILES "")
	LIST(APPEND FILES
	     __init__.py
	     Visualize.py
	     ShowProgress.py
	     MonitorResidual.py
	     MonitorDampingForce.py
	     MeasureEnergy.py)
	
     	FOREACH(FILE ${FILES})
		MACI_CONFIG_PY(${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.in 
		               ${CMAKE_CURRENT_BINARY_DIR}/plugins${DIM}/${FILE}.${DIM}.E
		               ${CMAKE_CURRENT_BINARY_DIR}/plugins${DIM}/${FILE}
		               plugins.${FILE}.${DIM})
	ENDFOREACH()

	INSTALL(DIRECTORY      ${CMAKE_CURRENT_BINARY_DIR}/plugins${DIM}
	        DESTINATION    lib/maci
	        FILES_MATCHING PATTERN "*.py")

ENDFOREACH()

