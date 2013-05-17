
# Export a particle list
# @param[in] 	filename	the output file
# @param[in]	kwargs		keywords 
def ExportParticleList(filename, **kwargs):
	fmt = "# ATOMDATA Id"

	# Get positions
	X   = kwargs["X"]
	fmt = fmt + " x=3"
	# Get velocities
	try:
		V   = kwargs["V"]
		fmt = fmt + " u=3"
	except:
		V = None
	# Get forces
	try:
		F   = kwargs["F"]
		fmt = fmt + " F=3"
	except:
		F = None
	# Get masses
	try:
		M = kwargs["M"]
		warn("Cannot handle masses in the export routine since the " \
		     "Tremolo format doesn't support masses.")
	except:
		M = None

	f = open(filename, "w")
	f.write(fmt + " type\n")

	for i,p in enumerate(X):
		f.write("%d" % i + "\t%22.15f\t%22.15f\t%22.15f\t" % tuple(p))
		if None != V:
			f.write("%22.15f\t%22.15f\t%22.15f\t" % tuple(V[i]))
		if None != F:
			f.write("%22.15f\t%22.15f\t%22.15f\t" % tuple(F[i]))
		f.write("P\n")

	f.close()
