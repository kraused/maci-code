
MACRO(MACI_ADD_LIBRARY TARGET TYPE SOURCES2 SOURCES3 LIBS2 LIBS3)
	
	ADD_LIBRARY(${TARGET}2 ${TYPE} ${SOURCES2})
	ADD_LIBRARY(${TARGET}3 ${TYPE} ${SOURCES3})

	TARGET_LINK_LIBRARIES(${TARGET}2 ${LIBS2})
	TARGET_LINK_LIBRARIES(${TARGET}3 ${LIBS3})

	IF("STATIC" STREQUAL "${TYPE}")
		SET_TARGET_PROPERTIES(${TARGET}2 PROPERTIES COMPILE_FLAGS "-fPIC -DDIM=2 ${ARGN}")
		SET_TARGET_PROPERTIES(${TARGET}3 PROPERTIES COMPILE_FLAGS "-fPIC -DDIM=3 ${ARGN}")
	ELSE()
		SET_TARGET_PROPERTIES(${TARGET}2 PROPERTIES COMPILE_FLAGS "-DDIM=2 ${ARGN}")
		SET_TARGET_PROPERTIES(${TARGET}3 PROPERTIES COMPILE_FLAGS "-DDIM=3 ${ARGN}")
	ENDIF()

	IF("SHARED" STREQUAL "${TYPE}")
		INSTALL(TARGETS ${TARGET}2 LIBRARY DESTINATION lib/maci)
		INSTALL(TARGETS ${TARGET}3 LIBRARY DESTINATION lib/maci)
	ELSE()
		INSTALL(TARGETS ${TARGET}2 ARCHIVE DESTINATION lib/maci)
		INSTALL(TARGETS ${TARGET}3 ARCHIVE DESTINATION lib/maci)
	ENDIF()

ENDMACRO()

