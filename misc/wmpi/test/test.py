
import mpi

mpi.init()

print mpi.world.rank()
print mpi.world.size()

comm = mpi.world.split(mpi.world.rank()%2, mpi.world.rank())
print comm.size()

mpi.finalize()

