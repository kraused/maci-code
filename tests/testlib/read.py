
import re

# Read and preprocess (remove comments)
# a JSON file
# Note that comments are not part of the
# JSON standard because it is only meant
# for serialization (basically)

def read_JSON(filename):
	lines = open(filename, "r").read().split("\n")
	str   = ""

	for line in lines:
		str += re.sub("#.*$", "", line)

	return str

