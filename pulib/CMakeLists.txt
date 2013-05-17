
ADD_LIBRARY(pu STATIC
	    puBasisFunction
	    puPatch
	    puQuad
	    puSpace
	    puTree
	    puVisualize)
SET_TARGET_PROPERTIES(pu PROPERTIES COMPILE_FLAGS -fPIC)

