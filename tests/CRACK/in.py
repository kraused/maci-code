
# Time measurements for testing
mpi.world.barrier()
t = mpi.wtime()

maci.main(mpi.world, maci.SPECFILE)

# Write a timestamp to indicate that we went through
mpi.world.barrier()
if 0 == mpi.world.rank():
	open("OK", "w").write("%f\n" % (mpi.wtime()-t))

