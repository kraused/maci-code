
MACRO(MACI_CONFIG_PY INPUT INTERMEDIATE OUTPUT TARGET)
	# Target names are not allowed to contain slashes
	# so we need to remove them
	STRING(REGEX REPLACE "/" "+" TARGETNAME ${TARGET})
		
	CONFIGURE_FILE(${INPUT} ${INTERMEDIATE})
	ADD_CUSTOM_COMMAND(
		OUTPUT  ${OUTPUT}
		COMMAND ${CMAKE_HOME_DIRECTORY}/python/ppp.rb
		ARGS    ${INTERMEDIATE}
			${OUTPUT}
		DEPENDS ${INTERMEDIATE})
	ADD_CUSTOM_TARGET(${TARGETNAME}
		ALL DEPENDS ${OUTPUT})
ENDMACRO()

