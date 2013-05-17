
// vi: tabstop=4:expandtab

#ifndef MACI_META_ASSEMBLER_IMPLEMENTATION_HH_INCLUDED
#define MACI_META_ASSEMBLER_IMPLEMENTATION_HH_INCLUDED 1

#include <string>
#include <map>

#include "Assembler.hh"
#include "TreeAssembler.hh"


/// MetaAssemblerImplementation: Implementation of the MetaAssembler class
/// The class MetaAssembler itself holds only a static pointer to the implementation.
class MetaAssemblerImplementation
{

public:
    /// Destroy the instance
    ~MetaAssemblerImplementation();

    /// Add a new assembler. The function returns 0 if
    /// the assembler is successfully added and one if an assembler
    /// with the same name already exists in the database
    int Add(const std::string& name, Assembler *x);

    /// Delete an assembler
    void Delete(const std::string& name);

    /// Get pointer to an assembler or NULL if the assembler is not
    /// found.
    Assembler *operator[](const std::string& name);

private:
    /// Each assembler is associated to a name to allow different classes
    /// to get assembler by their name
    std::map<std::string, Assembler*> assembler;
};

#endif

