
# Export a particle list
# @param[in] 	filename	the output file
# @param[in]	kwargs		keywords 
def ExportParticleList(filename, **kwargs):
	# Get positions
	X   = kwargs["X"]
	# Get the box
	box = kwargs["box"]

	f       = open(filename, "w")
	newline = lambda: f.write("\n")

	f.write(" LAMMPS File created by export.py\n")
	newline()

	f.write(" %d atoms\n" 		\
		" %d bonds\n" 		\
		" %d angles\n"		\
		" %d dihedrals\n"	\
		" %d impropers\n\n"	\
		" %d atom types\n"	\
		" %d angle types\n"
		" %d improper types\n" % (len(X), 0, 0, 0, 0, 1, 0, 0))
	newline()

	f.write(" %f\t%f\txlo xhi\n"  % (box[0], box[3]))
	f.write(" %f\t%f\tylo yhi\n"  % (box[1], box[4]))
	f.write(" %f\t%f\tzlo zhi\n"  % (box[2], box[5]))
	newline()

	f.write(" Masses\n")
	newline()

	# TODO How to handle multiple masses here?
	f.write(" %d %f\n" % (1, 1.0))
	newline()

	f.write(" Atoms\n")
	newline()

	for i,p in enumerate(X):
		f.write(" %d\t%d" % (i+1, 1) + \
			"\t%22.15f\t%22.15f\t%22.15f\n" % tuple(p))

	f.close()
