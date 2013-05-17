#
# Maci_TPL_UG.cmake: UG Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_UG "Enable UG" OFF)

MACI_TPL_REPORT(UG)
IF(NOT TPL_ENABLE_UG)
	RETURN()
ENDIF()

# The following variables must be set on the command
# line:
#
# - UG_INCLUDE_DIRS:PATH
# - UG_LIBRARY_DIRS:PATH
#
MACI_TPL_CHECK_INPUT(UG)

INCLUDE_DIRECTORIES(${UG_INCLUDE_DIRS})
LINK_DIRECTORIES   (${UG_LIBRARY_DIRS})

LIST(APPEND LIBS2 fe2)
LIST(APPEND LIBS3 fe3)

LIST(APPEND SRC2 fe/ug/Solver.cc
                 fe/ug/Mesh.cc
                 fe/ug/Element.cc
                 fe/ug/NodeDistributionMap.cc
                 fe/ug/NodalVector.cc
                 fe/ug/Energy.cc
                 fe/ug/src/SolverImplementation.cc
                 fe/ug/src/NeumannForceCalculator.cc
				 fe/ug/src/DirichletForceCalculator.cc
                 fe/ug/src/DirichletEnforcer.cc
                 fe/ug/src/ugTimeIntegrator.cc
                 fe/ug/src/ugVerletIntegrator.cc
                 fe/ug/src/ugRattleIntegrator.cc)
LIST(APPEND SRC3 fe/ug/Solver.cc
                 fe/ug/Mesh.cc
                 fe/ug/Element.cc
                 fe/ug/NodeDistributionMap.cc
                 fe/ug/NodalVector.cc
                 fe/ug/Energy.cc
                 fe/ug/src/SolverImplementation.cc
                 fe/ug/src/NeumannForceCalculator.cc
				 fe/ug/src/DirichletForceCalculator.cc
                 fe/ug/src/DirichletEnforcer.cc
                 fe/ug/src/ugTimeIntegrator.cc
                 fe/ug/src/ugVerletIntegrator.cc
                 fe/ug/src/ugRattleIntegrator.cc)

SET(HAVE_UG 1)

