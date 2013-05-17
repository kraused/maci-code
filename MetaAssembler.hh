
// vi: tabstop=4:expandtab

#ifndef MACI_META_ASSEMBLER_HH_INCLUDED
#define MACI_META_ASSEMBLER_HH_INCLUDED 1

#include <string>

#include "MetaAssemblerImplementation.hh"
#include "ProcElement.hh"
#include "Logging.hh"


/// MetaAssembler: Stores references to all assembler used in the program
/// One of the problems which need to be solved to allow for an efficient
/// design of the code (without killing structure) is how to handle the
/// assembling of matrices which are reused: The scale-transfer matrix is
/// used to compute the TransferOperator instance, the MultiplierMatrix
/// etc. On the one hand, the TransferOperator class can store the matrix
/// values in a fashion which make it inaccessible to other classes (e.g.,
/// by passing the data to a Sparse BLAS implementation which only provides
/// a limited interface), on the other hand frequent reassembling of the
/// values is very inefficient. Hence, the MetaAssembler was introduced.
/// It stores (name, reference) tuples for all assembler used in the code
/// so that the assembler for the MultiplierMatrix can reuse the assembler
/// for the scale transfer. Assembling happens on demand, i.e., depending
/// on the users choices different assembler the scale transfer assembler
/// can be created by the assembler for the MultiplierMatrix or by the
/// assembler for the FineFluctuationFilter. Of course, in both cases the
/// same result will be achieved. Since there is a standardized output
/// format for all assembles (COO format with tuples (rank, local index) 
/// as indices, it is easy to reuse the computed values.
/// There are certain cases (e.g., in FineFluctuationFilter) where it
/// is not possible to maintain this scheme. The reason is that in principle
/// any operation on assembled data must be performed on a copy of the
/// output matrix of the assembler since otherwise any subsequent use of
/// the assembled data will give incorrect results. On the other hand, 
/// the FineFluctuationMatrix can be really large and copying is not possible
/// with a fixed amount of GB/core. For this reason we provide a method to
/// delete assemblers. Any subsequent use of the same assembler will trigger
/// a rebuild of the data.
class MetaAssembler
{

public:
    MetaAssembler();

    /// Destroy the instance. Memory is only released if this is the
    /// last reference.
    ~MetaAssembler();

    /// Add a new assembler. Note that adding the assembler to the meta-assembler
    /// changes the owner-ship, i.e., the user is not allowed to the delete the
    /// Assembler itself. This is handled by the MetaAssembler instance. The
    /// function returns zero if the assembler was successfully added and one
    /// if an assembler with the same name already exists
    int Add(const std::string& name, Assembler *x);

    /// Delete an assembler from the list. Any subsequent use of the assembler
    /// will fail. However, when using the features provided by AssemblyProduct
    /// a rebuild of the assembler will be force. As mentioned above this is needed
    /// to cope with situations where we destroy the assembled data for
    /// efficiency reasons.
    void Delete(const std::string& name);

    /// Get pointer to an assembler. If the assembler cannot be found
    /// zero is returned.
    Assembler *operator[](const std::string& name);

private:
    /// Number of references
    static int numInstances;
    /// Pointer to the implementation
    static MetaAssemblerImplementation* impl;
};

#endif

