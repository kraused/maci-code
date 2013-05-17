
ADD_LIBRARY(hash STATIC
	    hashmem
	    hashtab
            lookupa
            recycle)
SET_TARGET_PROPERTIES(hash PROPERTIES COMPILE_FLAGS -fPIC)

