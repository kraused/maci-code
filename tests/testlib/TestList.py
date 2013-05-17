
import os

from Test import Test
from read import read_JSON

# TestList: A list of tests to execute
class TestList(list):

	# Create a list of tests from a file. The filename
	# should be absolute
	def __init__(self, filename):
		pwd = os.path.dirname(filename)
		[ self.append(Test(pwd, v)) for v in self.split(read_JSON(filename)) ]

	# The python-json library we use can only handle
	# one JSON object at a time. Hence, we need first to
	# split the input file into the different (top-level
	# objects -- we don't want to split for objects which are
	# part of other objects) strings
	def split(self, string):
		spl = []

		lvl = 0
		str = ""

		for c in string:
			str = str + c

			if "{" == c:
				lvl = lvl + 1
			if "}" == c:
				lvl = lvl - 1

				if 0 == lvl:
					spl.append(str)
					str = ""

		return spl

