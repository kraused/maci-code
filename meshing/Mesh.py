
import copy
import math
import sys
import xml.dom
import xml.dom.minidom

from Cuboid	import *
from CuboidList import *

import meshing

# The mesh. The geometry is described by a collection of patches
class Mesh(CuboidList):

	# Create a new instance
	def __init__(self, *args):
		CuboidList.__init__(self)

		self.nd  = []
		self.el  = []
		self.dim = 0

		assert(0 == len(args) or 1 == len(args))
		if(1 == len(args)):
			self.readXML(args[0])

	# Create the coarse mesh skeleton
	def createCoarseSkeleton(self):
		B = self.boundingBox()

		P = []

		for q in self:
			Q = [ q ]
			for p in self:
				D = min(q.dim, p.dim)
				for d in range(0,D):
					left = copy.deepcopy(B)
					left.upper[d] = p.lower[d]

					middle = copy.deepcopy(B)
					middle.lower[d] = p.lower[d]
					middle.upper[d] = p.upper[d]

					right = copy.deepcopy(B)
					right.lower[d] = p.upper[d]
		
					assert math.fabs(B.vol() - (left.vol()+middle.vol()+right.vol())) < 1e-5

					mk = []
					Z  = []

					for r in Q:
						cleft   =   left.intersect(r)
						cmiddle = middle.intersect(r)
						cright  =  right.intersect(r)

						n = 0

						if None != cleft   and   cleft.vol() > 1e-5:
							Z.append(cleft)
							n = n + 1

						if None != cmiddle and cmiddle.vol() > 1e-5:
							Z.append(cmiddle)
							n = n + 1

						if None != cright  and  cright.vol() > 1e-5:
							Z.append(cright)
							n = n + 1

						if n > 0:
							mk.append(r)

					[ Q.remove(r) for r in mk]
					[ Q.append(z) for z in Z ]

			[ P.append(q) for q in Q ]
						
		[self.pop()     for p in range(0,len(self))]
		[self.append(p) for p in P]

	# Mesh a single patch
	# @param p	the patch
	# @param h	approximate mesh size
	def meshPatch(self, p, h):
		n = [ 1, 1, 1 ]
		for d in range(0, p.dim):
			n[d] = max(1, int( (p.upper[d]-p.lower[d])/h ))

		w = [ 0.0, 0.0, 0.0 ]
		for d in range(0, p.dim):
			w[d] = (p.upper[d]-p.lower[d])/n[d]

		el = []
		for i in range(0,n[0]):
			for j in range(0,n[1]):
				for k in range(0,n[2]):

					e = Cuboid(p.dim, [ p.lower[0] +  i   *w[0],
						            p.lower[1] +  j   *w[1],
						            p.lower[2] +  k   *w[2] ],
						   	  [ p.lower[0] + (i+1)*w[0],
						            p.lower[1] + (j+1)*w[1],
						            p.lower[2] + (k+1)*w[2] ])

					el.append(e)

		return el
	
	# Collect the node set
	def collectNodes(self):
		nd = []
		for p in self:
			for i in range(0,p.ncorners()):
				for d in (0, 1, 2):
					nd.append(p.corner(i)[d])
		t = []
		meshing.point_list_uniqify(nd, t)

		for i in range(0, len(t)/3):
			self.nd.append([ t[3*i], t[3*i+1], t[3*i+2] ])

	# Define the elements
	def defineElements(self):
		nd = []
		for n in self.nd:
			for d in (0, 1, 2):
				nd.append(n[d])
		c_list = meshing.point_list_create(nd)

		for p in self:
			e = []
			for i in range(0,p.ncorners()):
				e.append(meshing.point_list_find(c_list, p.corner(i)))
			self.el.append(e)

		meshing.point_list_delete(c_list)

	# Create the finite element mesh
	# @param h	the (approximate) mesh size
	def createMesh(self, h):
		self.createCoarseSkeleton()

		el = []
		for p in self:
			[el.append(e) for e in self.meshPatch(p,h)]

		[self.pop()     for p in range(0,len(self))]
		[self.append(e) for e in el]

		print(" # patches in coarse skeleton: %d" % len(self))
		print("")

		self.collectNodes()
		self.defineElements()

		print(" # nodes:    %d" % len(self.nd))
		print(" # elements: %d" % len(self.el))
		print("")

	# Print to polydata file
	# @param filename	output file name
	def printPD(self, filename):
		dim = 0
		for p in self:
			dim = max(dim, p.dim)

		fh = open(filename,"w")
		for p in self:
			if 2 == dim:
				print >> fh, " QUAD %f %f %f %f %f %f %f %f %f %f %f %f 0.0" % \
					(p.corner(0)[0], p.corner(0)[1], p.corner(0)[2],
					 p.corner(1)[0], p.corner(1)[1], p.corner(1)[2],
					 p.corner(2)[0], p.corner(2)[1], p.corner(2)[2],
					 p.corner(3)[0], p.corner(3)[1], p.corner(3)[2])
			if 3 == dim:
				print >> fh, " HEX %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f 0.0" % \
					(p.corner(0)[0], p.corner(0)[1], p.corner(0)[2],
					 p.corner(1)[0], p.corner(1)[1], p.corner(1)[2],
					 p.corner(2)[0], p.corner(2)[1], p.corner(2)[2],
					 p.corner(3)[0], p.corner(3)[1], p.corner(3)[2],
					 p.corner(4)[0], p.corner(4)[1], p.corner(4)[2],
					 p.corner(5)[0], p.corner(5)[1], p.corner(5)[2],
					 p.corner(6)[0], p.corner(6)[1], p.corner(6)[2],
					 p.corner(7)[0], p.corner(7)[1], p.corner(7)[2])
		for i,n in enumerate(self.nd):
			print >> fh, " POINT %f %f %f %f" % (n[0], n[1], n[2], float(i))

		fh.close()

	# Read a mesh from an XML file
	# @param filename	input file name
	def readXML(self, filename):
		dom = xml.dom.minidom.parse(filename)

		msh = dom.getElementsByTagName('mesh')
		if(len(msh) < 1):
			sys.exit(" ERROR: No element with tag name 'mesh' found!")

		self.dim = int  (msh[0].getAttribute('dim'))
		h        = float(msh[0].getElementsByTagName('h')[0].childNodes[0].data.strip())

		for p in msh[0].getElementsByTagName('patch'):
			x = []
			[ x.append(float(z)) for z in p.childNodes[0].data.strip().split(' ')]
			self.add(Cuboid(self.dim,x[0:3],x[3:6]))

		dom.unlink()

		self.createMesh(h)

	# Write the mesh to an XML file
	# @param filename	output file name
	def writeXML(self, filename):
		doc = xml.dom.minidom.Document()

		msh = doc.createElement("mesh")
		msh.setAttribute("dim", "%s" % self.dim)

		nds = doc.createElement("nodes")
		msh.appendChild(nds)		

		for v in self.nd:
			n = doc.createElement("node")
			n.appendChild(doc.createTextNode("%f %f %f" % tuple(v)))
			nds.appendChild(n)

		els = doc.createElement("elements")
		msh.appendChild(els)		

		for v in self.el:
			data = ""
			for k in v:
				data = "%s %d" % (data,k)

			e = doc.createElement("element")
			e.appendChild(doc.createTextNode(data.strip()))
			els.appendChild(e)

		doc.appendChild(msh)

		fh = open(filename, "w")
		doc.writexml(fh, "", "\t", "\n", "UTF-8")
		fh.close()

		# We need to chop of the first line to remove the <?xml ...>
		# stuff so that we can include the mesh file wherever we want
		t = "\n".join(open(filename, "r").read().split("\n")[1:])
		open(filename, "w").write(t)

