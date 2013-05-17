#
# Maci_TPL_Tremolo.cmake: Tremolo Third-Party Package
#

INCLUDE(MACI_TPL_CHECK_INPUT)
INCLUDE(MACI_TPL_REPORT)

OPTION(TPL_ENABLE_Tremolo "Enable Tremolo" OFF)

MACI_TPL_REPORT(Tremolo)

# The following variables must be set on the command
# line:
#
# - Tremolo_INCLUDE_DIRS:PATH
# - Tremolo_LIBRARY_DIRS:PATH
#
MACI_TPL_CHECK_INPUT(Tremolo)

INCLUDE_DIRECTORIES(${Tremolo_INCLUDE_DIRS})
LINK_DIRECTORIES   (${Tremolo_LIBRARY_DIRS})

LIST(APPEND LIBS2 tremolo)
LIST(APPEND LIBS3 tremolo)

LIST(APPEND SRC2 md/tremolo/ParticleHandle.cc
                 md/tremolo/System.cc
                 md/tremolo/src/CallbackCompilationUnit.cc
                 md/tremolo/src/tremoloInstance.cc
                 md/tremolo/src/tremoloTimeIntegrator.cc
                 md/tremolo/src/tremoloVerletIntegrator.cc
                 md/tremolo/src/tremoloRattleIntegrator.cc)
LIST(APPEND SRC3 md/tremolo/ParticleHandle.cc
                 md/tremolo/System.cc
                 md/tremolo/src/CallbackCompilationUnit.cc
                 md/tremolo/src/tremoloInstance.cc
                 md/tremolo/src/tremoloTimeIntegrator.cc
                 md/tremolo/src/tremoloVerletIntegrator.cc
                 md/tremolo/src/tremoloRattleIntegrator.cc)

SET(HAVE_TREMOLO 1)

