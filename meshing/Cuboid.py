
import meshing

# A cuboid
class Cuboid:

	# Create a new instance
	# @param d	dimension of the polytope
	# @param lower	the lower corner of the cuboid
	# @param upper	the upper corner of the cuboid
	def __init__(self, d, lower, upper):
		self.dim   = d
		self.lower = lower
		self.upper = upper

	# The midpoint of the cuboid
	def midpoint(self):
		return [ 0.5*(self.lower[0] + self.upper[0]),
			 0.5*(self.lower[1] + self.upper[1]),
			 0.5*(self.lower[2] + self.upper[2]) ]
	
	# Sidelengths of the cuboid
	def sidelenghts(self):
		return [ self.upper[0] - self.lower[0],
			 self.upper[1] - self.lower[1],
			 self.upper[2] - self.lower[2] ]

	# Concat an array of floats
	def __concat(self, *kargs):
		c = [ ]
		for x in kargs:
			for y in x:
				c.append(y)
		return c

	# Check if two cuboids have an intersection
	# @param o the other cuboid
	def hasIntersection(self, o):
		return meshing.box_have_intersection(self.dim,
				self.__concat(self.lower,self.upper),
				self.__concat(   o.lower,   o.upper))

	# Compute the intersection between two cuboids
	# @param o the other cuboid
	def intersect(self,o):
		if not self.hasIntersection(o):
			return None
	
		cut   = []
		meshing.box_intersection(self.dim,
				self.__concat(self.lower,self.upper),
				self.__concat(   o.lower,   o.upper),cut)

		return Cuboid(self.dim,cut[0:3],cut[3:6])

	# Compute the volume of the cuboid
	def vol(self):
		return meshing.box_volume(self.dim,
					self.__concat(self.lower,self.upper))

	# Print out a polytope
	def __str__(self):
		return "[ (%f, %f, %f), (%f, %f, %f) ]" % tuple(self.__concat(self.lower,self.upper))

	# Number of corners
	def ncorners(self):
		n = { 2 : 4, 3 : 8 }
		return n[self.dim]

	# Get a corner
	def corner(self, i):
		if 2 == self.dim:
			map = { 0 : [ self.lower[0], self.lower[1], self.lower[2] ],
				1 : [ self.upper[0], self.lower[1], self.lower[2] ],
				2 : [ self.upper[0], self.upper[1], self.lower[2] ],
				3 : [ self.lower[0], self.upper[1], self.lower[2] ] }
			return map[i]
		if 3 == self.dim:
			map = { 0 : [ self.lower[0], self.lower[1], self.lower[2] ],
				1 : [ self.upper[0], self.lower[1], self.lower[2] ],
				2 : [ self.upper[0], self.upper[1], self.lower[2] ],
				3 : [ self.lower[0], self.upper[1], self.lower[2] ],
				4 : [ self.lower[0], self.lower[1], self.upper[2] ],
				5 : [ self.upper[0], self.lower[1], self.upper[2] ],
				6 : [ self.upper[0], self.upper[1], self.upper[2] ],
				7 : [ self.lower[0], self.upper[1], self.upper[2] ] }
			return map[i]

