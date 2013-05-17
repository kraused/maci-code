
import sys
import copy
import os

# The input path: When opening input xml files, data files etc.
# we always check in these directories if the file exists and
# open the absolute path. In this way we can run a simulation from
# a sandbox while reading the input data from a different directory

# Set the input path. This function should be called only once in
# the initialization of MACI.
def SetInputPath(input_path):
	global INPUT_PATH
	INPUT_PATH = copy.deepcopy(input_path)

# An exception which is raised if a file is not found anywhere in the
# directories which are in the path
class FileNotFoundInPathException(Exception):
	def __init__(self, msg):
		self.__msg = msg
		pass
	def __str__(self):
		return self.__msg

# Find a file in the input path and return the absolut path to the
# file. If the file is not found in the path, an exception is raised.
def FindInPath(file):
	for v in INPUT_PATH:
		if os.path.exists(v + "/" + file):
			return v + "/" + file

	raise FileNotFoundInPathException("Could not find file \"%s\" in INPUT_PATH = %s." % (file, INPUT_PATH))

