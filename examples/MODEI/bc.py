
# MESH STATISTICS:
# DIM                    : 2
# NUM NODES              : 28712
# NUM ELEMENTS           : 28178
# AVG CORNERS PER ELEMENT: 4.0
# MINIMAL COORDINATE     : [ -1.305612e+02, 4.943877e+01, 0.000000e+00 ]
# MAXIMAL COORDINATE     : [ 5.623496e+02, 7.261573e+02, 0.000000e+00 ]

def b(x,y,z):
	if x < -130.5 or x > 562.0:
		return 1
	return 0

# Boundary forces
def F(x,y,z,t):
	if x < -130.5:
		return [ -0.25, 0.0, 0.0 ]
	if x > +562.0:
		return [ +0.25, 0.0, 0.0 ]
	return None

