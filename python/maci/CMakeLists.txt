
# ------------------------------------------------------------------------------
# Tremolo interface
# ------------------------------------------------------------------------------
IF(HAVE_TREMOLO)
	FOREACH(DIM 2 3 )
		MACI_CONFIG_PY(${CMAKE_CURRENT_SOURCE_DIR}/tremolo/MMSystem.py.in 
		               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/MMSystem.py.${DIM}.E
		               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/MMSystem.py
		               MMSystem.py.${DIM})
	ENDFOREACH()

	INSTALL(DIRECTORY      ${CMAKE_CURRENT_SOURCE_DIR}/tremolo
	        DESTINATION    lib/maci
	        FILES_MATCHING PATTERN "*.py")

	LIST(APPEND LIBS2 tremolo)
	LIST(APPEND LIBS3 tremolo)
ENDIF()

# ------------------------------------------------------------------------------
# Lammps interface
# ------------------------------------------------------------------------------
IF(HAVE_LAMMPS)
	FOREACH(DIM 2 3)
		MACI_CONFIG_PY(${CMAKE_CURRENT_SOURCE_DIR}/lammps/MMSystem.py.in 
		               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/MMSystem.py.${DIM}.E
		               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/MMSystem.py
		               MMSystem.py.${DIM})
	ENDFOREACH()

	INSTALL(DIRECTORY      ${CMAKE_CURRENT_SOURCE_DIR}/lammps
	        DESTINATION    lib/maci
	        FILES_MATCHING PATTERN "*.py")

	LIST(APPEND LIBS2 ${Lammps_LIBRARY_NAMES} ${fftw_LIBRARY_NAMES})
	LIST(APPEND LIBS3 ${Lammps_LIBRARY_NAMES} ${fftw_LIBRARY_NAMES})
ENDIF()

# ------------------------------------------------------------------------------
# UG interface
# ------------------------------------------------------------------------------
IF(HAVE_UG)
	SET (FILES "")
	LIST(APPEND FILES
	     ug/__init__.py
	     ug/Mesh.py)

	FOREACH(DIM 2 3)
		MACI_CONFIG_PY(${CMAKE_CURRENT_SOURCE_DIR}/ug/FESolver.py.in 
		               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/FESolver.py.${DIM}.E
		               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/FESolver.py
		               FESolver.py.${DIM})

		FOREACH(FILE ${FILES})
			MACI_CONFIG_PY(${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.in 
			               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/${FILE}.${DIM}.E
			               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/${FILE}
			               ${FILE}.${DIM})
		ENDFOREACH()
	ENDFOREACH()

	INSTALL(DIRECTORY      ${CMAKE_CURRENT_SOURCE_DIR}/ug
	        DESTINATION    lib/maci
	        FILES_MATCHING PATTERN "*.py")
ENDIF()

# ------------------------------------------------------------------------------
# The maci library
# ------------------------------------------------------------------------------
MACI_PROCESS_SWIG(maciW2.cc ${CMAKE_CURRENT_SOURCE_DIR}/maci.i DIM 2)
MACI_PROCESS_SWIG(maciW3.cc ${CMAKE_CURRENT_SOURCE_DIR}/maci.i DIM 3)

SET (SOURCE2 "")
LIST(APPEND SOURCE2 maciW2.cc
                    WeightsPyObj.cc)
SET (SOURCE3 "")
LIST(APPEND SOURCE3 maciW3.cc
                    WeightsPyObj.cc)

MACI_ADD_LIBRARY(maciS SHARED "${SOURCE2}" "${SOURCE3}" "maci2;${LIBS2};${PYTHON_LIBRARY}" "maci3;${LIBS3};${PYTHON_LIBRARY}" "-Wno-uninitialized")

# Install Python files
INSTALL(FILES          ${CMAKE_CURRENT_BINARY_DIR}/maciS2.py
                       ${CMAKE_CURRENT_BINARY_DIR}/maciS3.py
        DESTINATION    lib/maci)

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
FOREACH(DIM 2 3)
	SET (FILES "")
	LIST(APPEND FILES
	     __init__.py 
	     ProcElement.py 
	     RattleCorrectorProxy.py 
	     ParticleList.py 
	     DampingMethodProxy.py
	     RattleIntegrator.py 
	     VerletIntegrator.py
	     MMSystemBase.py
	     FESolverBase.py
	     Projection.py
	     InterpolationWeights.py
	     PatchyHandshakeGeo.py
	     TimeIntegratorProxy.py
	     FilterProxy.py
	     PluginTable.py
	     main.py
	     initconds.py)

	FOREACH(FILE ${FILES})
		MACI_CONFIG_PY(${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.in 
		               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/${FILE}.${DIM}.E
		               ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}/${FILE}
		               ${FILE}.${DIM})
	ENDFOREACH()

	INSTALL(DIRECTORY      ${CMAKE_CURRENT_BINARY_DIR}/maci${DIM}
	        DESTINATION    lib/maci
	        FILES_MATCHING PATTERN "*.py")
	
	FILE(GLOB FILES 
	     ${CMAKE_CURRENT_SOURCE_DIR}/*.py)
	INSTALL(FILES           ${FILES}
	        DESTINATION     lib/maci)

ENDFOREACH()

