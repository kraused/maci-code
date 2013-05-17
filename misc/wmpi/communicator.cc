
// vi: tabstop=4:expandtab

#include "communicator.hh"

communicator::communicator()
: h(MPI_COMM_NULL)
{
}
	
communicator::communicator(MPI_Comm comm)
{
	if(h == comm) {
		h = MPI_COMM_NULL;
	} else {
		MPI_Comm_dup(comm, &h);
	}
}

communicator::~communicator()
{
	int finalized;
	MPI_Finalized(&finalized);

	if(finalized) {
		return;
	}	

	// TODO Check if MPI was already finalized
	if(MPI_COMM_NULL != h) {
		MPI_Comm_free(&h);
	}
}

communicator::communicator(const communicator &o)
{
	MPI_Comm_dup(o.h, &h);
}

communicator &communicator::operator=(const communicator &o)
{
	if(MPI_COMM_NULL != h) {
		MPI_Comm_free(&h);
	}
	MPI_Comm_dup (o.h, &h);
	return (*this);
}

int communicator::size()
{
	int numprocs;
	MPI_Comm_size(h, &numprocs);
	return numprocs;
}

int communicator::rank()
{
	int myrank;
	MPI_Comm_rank(h, &myrank);
	return myrank;
}

void communicator::abort(int code)
{
	MPI_Abort(h, code);
}

communicator communicator::split(int color, int key)
{
	MPI_Comm newcomm;
	MPI_Comm_split(h, color, key, &newcomm);

	communicator C(newcomm);

	if(MPI_COMM_NULL != newcomm) {
		MPI_Comm_free(&newcomm);
	}

	return C;
}

void communicator::barrier()
{
	MPI_Barrier(h);
}

MPI_Comm communicator::handle()
{
	return h;
}

