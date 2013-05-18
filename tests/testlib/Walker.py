
import os
import re
import test

from TestList import TestList

# The directory walker visits all directories and searches
# for "tests.json" files in there
class Walker:

	# Create a new walker with root directory root.
	def __init__(self, root):
		self.__root  = os.path.abspath(root)

		# List of tests collected while traversing
		# the directories
		self.__tests = []

	# Do the walking. Collect all tests on the way
	def walk(self):
		os.path.walk(self.__root, lambda arg, dirname, names: self._walk(arg, dirname, names), None)
		return self.__tests


	# Internal walker helper
	def _walk(self, dummy, dirname, names):
		# First of all we need to skip all these
		# svn folders that we do not really
		# care about
		if -1 != dirname.find(".svn"):
			return

		# Next we skip our own folder
		if -1 != dirname.find("testlib"):
			return
		# And the config folder
		if -1 != dirname.find("config"):
			return

#		# Create a copy of the directory in the target
#		# tree
#		newdir = os.path.abspath(dirname).replace(self.__root, target)
#
#		if False == os.path.exists(newdir):
#			os.mkdir(newdir)

		if True == os.path.exists(dirname + "/tests.json"):
			self.__tests.extend(TestList(dirname + "/tests.json"))

