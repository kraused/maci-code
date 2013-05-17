
// vi: tabstop=4:expandtab

#ifndef MACI_ASSEMBLER_HH_INCLUDED
#define MACI_ASSEMBLER_HH_INCLUDED 1

#include "AssemblerOutputMatrix.hh"


/// Assembler: Base class for all assemblers
///
/// Each assembler implements the assemble() function which creates
/// an assembled matrix in AssemblerOutputMatrix format. See
/// AssemblerOutputMatrix.hh for a description of the standard format
/// that assembler need to conform too.
class Assembler
{

public:
    /// Delete the instance
    virtual ~Assembler() {}

    /// Assemble the coordinate matrix
    virtual void assemble() = 0;

public:
    /// The output matrix in COO format. This member is 
    /// public so that other classes can use the assembled
    /// matrix
    AssemblerOutputMatrix matrix;
};

#endif

