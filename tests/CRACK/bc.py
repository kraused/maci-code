
def b(x,y,z):
	if x > 278.5 or x < -0.5:
		return 1
	return 0

# Boundary forces
def g(x,y,z,t):
	if x > 278.5:
		return [ +0.5, 0.0, 0.0 ]
	if x < -0.5:
		return [ -0.5, 0.0, 0.0 ]
	return None

