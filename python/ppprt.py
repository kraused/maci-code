
# Runtime library for the ppp Python preprocessor

import xml.dom.minidom
import re
import InputPath

# Load a file into a string
def ReadFile(name):
	return open(name,"r").read()

# Parse a file and return a minidom handle
def ParseFile(name):
	return ParseString(ReadFile(name))

# Parse a string and return a minidom handle
def ParseString(str):
	return xml.dom.minidom.parseString(Preprocess(str))

# Preprocess a string. The preprocessor allows us to implement
# some desirable features which themself are not standard or not
# covered by the minidom module
# Implemented features are:
# - Allow for including other xml files (recursively) via the
#   syntax
#		#include "the_other_xml_file.xml"
#   Note that this must be the only statement in this line.
#   TODO Currently includes cannot be commented out.
def Preprocess(str):
	# First we resolve includes
	str2 = ""
	for line in str.split("\n"):
		if -1 != line.find("#include"):
			# Use a regular expression to extract the file name. The () parentheses
			# tell the re module that this should be a group
			file  = re.search("#include\s+\"([a-zA-Z0-9.]+)\"", line).group(1)
			# Find the file in the path, open it and call recursively
			# the preprocess function so that we can allow for includes
			# also in files which have been themselve included.
			str2 += Preprocess(ReadFile(InputPath.FindInPath(file)))
		else:
			str2 += line

	return str2

# Find a direct children of a node. We cannot use
# getElementsByTagName for this because it searches
# recursively and we don't want this behaviour
def FindChildElements(node, name):
	els = []

	for c in node.childNodes:
#		if c.nodeType in [ c.ELEMENT_NODE, c.ATTRIBUTE_NODE, c.TEXT_NODE ]:
		if c.nodeName == name:
			els.append(c)

	return els

