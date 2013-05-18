
def b(x,y,z):
	if x > 442.5 or x < -76.5:
		return 1
	return 0

# Boundary forces
def F(x,y,z,t):
	if x >  442.5:
		return [ +0.25, 0.0, 0.0 ]
	if x < -76.5:
		return [ -0.25, 0.0, 0.0 ]
	return None

