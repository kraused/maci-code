
PROJECT( spblas )
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

SET(CMAKE_BUILD_TYPE Release)

ADD_LIBRARY          (spblas STATIC nist_spblas.cc)
SET_TARGET_PROPERTIES(spblas PROPERTIES COMPILE_FLAGS -fPIC)

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
SET(LIB_INSTALL_DIR     ${CMAKE_INSTALL_PREFIX}/lib)
SET(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)
SET(BIN_INSTALL_DIR     ${CMAKE_INSTALL_PREFIX}/bin)
SET(MAN_INSTALL_DIR     ${CMAKE_INSTALL_PREFIX}/share/man)

INSTALL(TARGETS             spblas 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

FILE   (GLOB HEADER *.h)
INSTALL(FILES           ${HEADER} 
        DESTINATION     include)

