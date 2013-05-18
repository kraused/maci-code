
def b(x,y,z):
	if x > 436 or x < -23.4:
		return 1
	return 0

# Boundary forces
def F(x,y,z,t):
	if x >  436:
		return [ +0.25, 0.0, 0.0 ]
	if x < -23.4:
		return [ -0.25, 0.0, 0.0 ]
	return None

