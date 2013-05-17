
// vi: tabstop=4:expandtab

#ifndef MACI_COORD_EXCHANGE_HH_INCLUDED
#define MACI_COORD_EXCHANGE_HH_INCLUDED 1

#include "linalg/CoordMatrix.hh"
#include "Int64.hh"

/// Gather matrix rows (associated to mesh nodes) at the
/// owner processing elements. The output matrix will contain
/// duplicate entries which need to be dealt with by the user
/// @param[in]  send    the input matrix
/// @param[in]  recv    contains the data received from all processes.
///                     It only contains rows for nodes owned by the
///                     processing element
void GatherRowsAtOwners(const linalg::CoordMatrix<int64, double>& send, linalg::CoordMatrix<int64, double>& recv);

#endif

