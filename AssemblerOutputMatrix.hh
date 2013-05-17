
// vi: tabstop=4:expandtab

#ifndef MACI_ASSEMBLER_OUTPUT_MATRIX_HH_INCLUDED
#define MACI_ASSEMBLER_OUTPUT_MATRIX_HH_INCLUDED 1

#include "Int64.hh"
#include "linalg/CoordMatrix.hh"


/// AssemblerOutputMatrix: The output format for each assembler
///
/// To allow for reuse of the values computed by assembler, a
/// standard has been devised for an output format that all
/// assembler classes have to follow. This intermediate format
/// can be reused and allows for easy translation into more
/// complicated (but better performing) storage formats like
/// CSR. 
/// The output matrix of an assembler is stored in double precision
/// as a matrix in COO format. The indices are 64-bit integers
/// consisting of the rank (first 32 bits) and the local index (last
/// 32 bits):
///     +------+-------------+
///     | rank | local index |
///     +------+-------------+
/// By rank we always mean the rank in the world communicator.
/// However, the meaning of the local index is not standardized:
/// It can be the local index of a mesh node in the bridging domain
/// or the local index in the full mesh. It is the responsibility
/// of the user to ensure that he does not plug together assembler
/// with different usage of "local index".
/// The coordinate output matrix is always stored non-overlapping.
/// This especially means that halo or ghost nodes on the FE side
/// are not taken into account. Also, the assembler class must take
/// care of nodes stored on multiple processing elements.
/// For block matrices where the values are non-zero only on the
/// diagonal of the block and constant along the diagonal we store
/// only one value per block.
/// Duplicate entries are not allowed in the output matrix.
typedef linalg::CoordMatrix<int64, double> AssemblerOutputMatrix;

#endif

