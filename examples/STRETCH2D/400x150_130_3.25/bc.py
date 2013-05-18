
def b(x,y,z):
	if y > 355.5 or y < -50.5:
		return 1
	return 0

# Boundary values
def U(x,y,z,t):
	if y >  355.5:
		return [ 0.0,  0.01*t, 0.0 ]
	if y < -50.5:
		return [ 0.0, -0.01*t, 0.0 ]
	return None

