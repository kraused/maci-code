
// vi: tabstop=4:expandtab

#include "wmpi.hh"

void init()
{
	MPI_Init(0,0);
	world = communicator(MPI_COMM_WORLD);
}

int finalize()
{
	return MPI_Finalize();
}

double wtime()
{
	return MPI_Wtime();
}

// The world communicator
communicator world;

