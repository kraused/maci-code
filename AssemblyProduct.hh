
// vi: tabstop=4:expandtab

#ifndef MACI_ASSEMBLY_PRODUCT_HH_INCLUDED
#define MACI_ASSEMBLY_PRODUCT_HH_INCLUDED

#include <string>

#include "Assembler.hh"
#include "DesignByContract.hh"
#include "MetaAssembler.hh"


/// AssemblyProduct: The base class for all objects which
///                  need to be assembled.
/// Every class which uses/consumes assembled data must be
/// derived from AssemblyProduct. 
/// Base class for all objects which need to be assembled
class AssemblyProduct
{

public:
    /// Default constructor
    AssemblyProduct();

    /// Destructor
    ~AssemblyProduct();

    /// Add the primary assembler with name name and. x must be a
    /// valid pointer to the instance.
    ///
    /// It is an error if the primary assembler is already in the 
    /// database. This makes sense as there should be a one-to-one
    /// correspondence between classes and assembler and many
    /// classes can only be constructed once (e.g. because multiple
    /// would mutually overwrite their data)
    ///
    /// TODO No it doesn't: The assembler might have been used as
    ///      a secondary assembler before. However, dropping this
    ///      assumption, what is the point in having primary and
    ///      secondary assembler?
    ///
    /// This function triggers the x->assemble() function
    void AddPrimaryAssembler(const std::string& name, Assembler *x);

    /// Add a secondary assembler for use by the primary assembler
    /// If an assembler with the same name has been added before
    /// the pointer x is freed. Otherwise x->assemble() is called
    void AddSecondaryAssembler(const std::string& name, Assembler *x);

    /// The macro AddSecondaryAssemblerIfNotExisting is a convenience
    /// wrapper around the functions AddSecondaryAssembler and 
    /// GetSecondaryAssembler. It first checks if the assembler
    /// exists and only calls AddSecondaryAssembler() if it doesn't exist
    /// If (p) is something like (new Tp(...)) the macro will make sure
    /// that the constructor of Tp is only called if necessary. In
    /// theory, assembler should be created in such a way that creating
    /// and destroying them is cheap. In this case it is okay to always
    /// call AddSecondaryAssembler with x equal to new Tp(...). However,
    /// the world is not perfect and it is unfortunately by design of some
    /// subcomponents currently not the case that creating and destroying
    /// assembler (especially TreeAssembler) is cheap. Hence we provide
    /// this macro.
    /// The this-> prefix to all calls is necessary if the macro
    /// is called from a template class.
#undef  AddSecondaryAssemblerIfNotExisting
#define AddSecondaryAssemblerIfNotExisting(name, p)         \
    do                                                      \
    {                                                       \
        if(!this->GetSecondaryAssembler(name))              \
            this->AddSecondaryAssembler((name), (p));       \
    }                                                       \
    while(0)

    /// Get a pointer to the primary assembler
    Assembler* GetPrimaryAssembler();

    /// Get a pointer to a secondary assembler
    Assembler *GetSecondaryAssembler(const std::string& name);

    /// Delete the primary assembler. If the class destroys (for
    /// reasons discussed in MetaAssembler.hh) the assembled output
    /// it makes sense to delete the assembler to ensure that no
    /// other class will accidentaly use invalid data
    void DeletePrimaryAssembler();

    /// Delete a secondary assembler.
    void DeleteSecondaryAssembler(const std::string& name);

private:
    /// The meta assembler. Essentially only one instance
    /// exists in the implementation. This allows for sharing
    /// assembler between different classes which is the reason
    /// why we created the assembler class structure
    MetaAssembler metaAssembler;
    /// Name of the primary assembler
    std::string priAssemblerName;
};

#endif

