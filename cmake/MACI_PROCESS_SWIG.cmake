
MACRO(MACI_MACRO_PARSE_ARG ARGS NAME VAL)
	SET(T FALSE)

	FOREACH(ARG ${ARGS})
		IF(T)
			SET(${VAL} ${ARG})
		ENDIF()
		IF(${NAME} STREQUAL "${ARG}")
			SET(T TRUE)
		ENDIF()
	ENDFOREACH()
ENDMACRO()

MACRO(MACI_PROCESS_SWIG OUTPUT INPUT)
	# Create the include path for SWIG
	GET_DIRECTORY_PROPERTY(IDIRS INCLUDE_DIRECTORIES)
	SET(FLAGS "")
	FOREACH(DIR ${IDIRS})
		LIST(APPEND FLAGS -I${DIR})
	ENDFOREACH()

	# Parse the DIM argument
	MACI_MACRO_PARSE_ARG("${ARGV}" "DIM" DIM)
	IF(DIM)
		LIST(APPEND FLAGS "-DDIM=${DIM}")
	ENDIF()

	ADD_CUSTOM_COMMAND(
		OUTPUT  ${OUTPUT}
		COMMAND ${SWIG_EXECUTABLE}
		ARGS    -python -c++ -shadow -DHAVE_CONFIG_HH=1 ${FLAGS} -o ${OUTPUT}
			${INPUT}
		DEPENDS ${INPUT})

ENDMACRO()

