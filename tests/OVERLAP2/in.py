
# Time measurements for testing
mpi.world.barrier()
t = mpi.wtime()

# Note: The initialization sequence below
#	is mostly taken from the maci.main()
#	function. Take a look at the main.py.in
#	file in python/maci for more
#	documentation

app = maci.Maci()

thetype = maci.petype(mpi.world, maci.SPECFILE)
mpi.world.barrier()

subcomm = mpi.world.split(thetype, mpi.world.rank())
t0      = None
if maci.ProcMD == thetype:
	t0 = maci.MMSystem(subcomm, maci.SPECFILE)
	app.SetSystem(t0)
if maci.ProcFE == thetype:
	t0 = maci.FESolver(subcomm, maci.SPECFILE)
	app.SetSolver(t0)
mpi.world.barrier()

# Initialize the procel structure
app.procel = maci.ProcElement(mpi.world, app, maci.SPECFILE)
mpi.world.barrier()

# Create the handshake geometry
app.handshake = maci.PatchyHandshakeGeo(app, maci.SPECFILE)
mpi.world.barrier()

# Create the weighting function
app.weights = maci.InterpolationWeights(app, maci.SPECFILE)
mpi.world.barrier()

# Assemble matrices, fill in the piggyback data ...
t0.init(app)
mpi.world.barrier()

# P = maci.Projection (app, maci.SPECFILE)
# F = maci.FilterProxy(app, maci.SPECFILE)
# 
# if maci.ProcMD == app.procel.mytype:
# 	t0.ApplyFunctionVel(lambda x, y, z: 1.0)
# 
# P.project(app.procel, "A")
# 
# if maci.ProcMD == app.procel.mytype:
# 	import math
# 	t0.ApplyFunctionVel(lambda x,y,z: math.sin(x*0.05*3.14159)*math.cos(y*0.05*3.14159))
# 	
# P.project(app.procel, "B")
# F.apply  (app.procel, "C")

# Write a timestamp to indicate that we went through
mpi.world.barrier()
if 0 == mpi.world.rank():
	open("OK", "w").write("%f\n" % (mpi.wtime()-t))

