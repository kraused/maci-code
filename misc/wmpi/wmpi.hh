
// vi: tabstop=4:expandtab

#ifndef WMPI_WMPI_HH_INCLUDED
#define WMPI_WMPI_HH_INCLUDED 1

#include "communicator.hh"

/// Initialize the MPI library
void init();

/// Finalize the MPI library
int finalize();

/// Get wall clock time
double wtime();

/// The world communicator
extern communicator world;

#endif
