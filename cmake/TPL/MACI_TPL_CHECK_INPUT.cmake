
MACRO(MACI_TPL_CHECK_INPUT TPLNAME)

	IF(NOT ${TPLNAME}_INCLUDE_DIRS)
		MESSAGE(FATAL_ERROR "${TPLNAME}_INCLUDE_DIRS not set")
	ENDIF()
	IF(NOT ${TPLNAME}_LIBRARY_DIRS)
		MESSAGE(FATAL_ERROR "${TPLNAME}_LIBRARY_DIRS not set")
	ENDIF()
   	# The names cannot in general be given on the comand line because
    # different libraries might be intended for different dimensions
	
	IF( "NAMES" STREQUAL "${ARGN}")
		IF(NOT ${TPLNAME}_LIBRARY_NAMES)
			MESSAGE(FATAL_ERROR "${TPLNAME}_LIBRARY_NAMES not set")
		ENDIF() 
	ENDIF()

ENDMACRO()

