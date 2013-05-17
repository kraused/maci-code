
from Cuboid import *

# A list of cuboids
class CuboidList(list):

	# Create a new instance
	# param cubs	a list of cuboids
	def __init__(self):
		list.__init__(self)

	# Compute the bounding box of all cuboids in the list
	def boundingBox(self):
		if 0 == len(self):
			return None

		lower = self[0].lower
		upper = self[0].upper
		d     = 0

		for p in self:
			lower = min(lower, p.lower)
			upper = max(upper, p.upper)
			d     = max(d    , p.dim  )

		return Cuboid(d, lower, upper)

	# Add a set of elements
	# @param args	list of elements
	def add(self, *args):
		for p in args:
			self.append(p)
