
// vi: tabstop=4:expandtab

#ifndef WMPI_COMMUNICATOR_HH_INCLUDED
#define WMPI_COMMUNICATOR_HH_INCLUDED 1

#include <mpi.h>

class communicator
{

public:
	/// The default constructor
	communicator();
	
	/// Create a new instance from a communicator handle
	communicator(MPI_Comm comm);

	/// Delete the instance
	~communicator();

	/// Copy constructor
	/// @param o	the other object
	communicator(const communicator &o);

	/// Assignement operator
	/// @param o	the other object
	communicator &operator=(const communicator &o);

	/// Query the size of the communicator
	int size();

	/// Query the rank of the procesor
	int rank();

	/// Abort the execution
	/// @param code	the exit code
	void abort(int code);

	/// Split the communicator
	communicator split(int color, int key);

	/// Barrier synchronization
	void barrier();

	/// Get the MPI handle. This function is necessary for passing MPI handles
	/// between different (unrelated) modules
	MPI_Comm handle();

private:
	/// The MPI object handle
	MPI_Comm h;
};

#endif
