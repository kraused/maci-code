
import math

# Create a hexagonal lattice and return the particle list 
# @param[in]	N	estimated number of particles in each dimension
# @param[in]	rho	offset
# @param[in]	dec	only particles for which the function dec evaluates to
#			true are included in the particle list
def CreateHex2ParticleList(N, rho, dec = lambda p: True):
	P     = []
	sqrt3 = 1.73205080756888
	req   = 1.12246204830937

	def add(x,y,P):
		if True == dec([x, y]): 
			P.append([x, y, rho[2]])

	x = rho[0]
	for i in range(1,int(math.ceil(float(N[0]/math.sqrt(2))))):
		y = rho[1]
		for j in range(1,int(math.ceil(float(N[1]/math.sqrt(2))))):
			add(x,y,P)

            		y = y + sqrt3*req
        	x = x + req

	x = rho[0] + 0.5*req
	for i in range(1,int(math.ceil(float(N[0]/math.sqrt(2))))):
		y = rho[1] + 0.5*sqrt3*req
		for j in range(1,int(math.ceil(float(N[1]/math.sqrt(2))))):
			add(x,y,P)

			y = y + sqrt3*req
		x = x + req

	return P

# Create an fcc lattice and return the particle list 
# @param[in]	N	estimated number of particles in each dimension
# @param[in]	rho	offset
# @param[in]	dec	only particles for which the function dec evaluates to
#			true are included in the particle list
def CreateFcc3ParticleList(N, rho, dec = lambda p: True):
	P     = []
	req   = 1.5509

	def add(x,y,z,P):
		for i,j,k in [ (0,0,0), (1,1,0), (1,0,1), (0,1,1) ]:
			xp = x + i*0.5*req
			yp = y + j*0.5*req
			zp = z + k*0.5*req

			if True == dec([xp, yp, zp]):
				P += [ [ xp, yp, zp ] ]

	for i in range(0,N[0]):
		for j in range(0,N[1]):
			for k in range(0,N[2]):
				add(rho[0] + i*req, rho[1] + j*req, rho[2] + k*req,P)

	return P

# Compute the bounding box for a particle list
# @param[in]	P	particle list
# @param[in]	rho	offset
def BoundingBoxOfParticleList(P, rho):
	x = 0.0
	y = 0.0
	z = 0.0

	for v in P:
		x = max(x, v[0])
		y = max(y, v[1])
		z = max(z, v[2])	

	return [ 0.0, 0.0, 0.0, x+rho[0], y+rho[1], z+rho[2] ]
