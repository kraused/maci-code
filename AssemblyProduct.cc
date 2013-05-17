
// vi: tabstop=4:expandtab

#include "AssemblyProduct.hh"
#include "Logging.hh"


AssemblyProduct::AssemblyProduct()
{
}

AssemblyProduct::~AssemblyProduct()
{
}

void AssemblyProduct::AddPrimaryAssembler(const std::string& name, Assembler *x)
{
    CHECK_PRECOND_ALWAYS(x);
    
    /// TODO No difference anymore between primary and secondary assembler?
    AddSecondaryAssembler(name, x);

    priAssemblerName = name;
}

void AssemblyProduct::AddSecondaryAssembler(const std::string& name, Assembler *x)
{
    CHECK_PRECOND_ALWAYS(x);

    if(1 == metaAssembler.Add(name, x))
        delete x;       /// The assembler already exists and the data has
                        /// be computed previously. No need to do it again
    else
    {
        Log::Instance().write("Starting assembly process for \"%s\"\n", name.c_str());
        x->assemble();  /// No such assembler: Assemble the data now
    }
}

void AssemblyProduct::DeletePrimaryAssembler()
{
    metaAssembler.Delete(priAssemblerName);
}

void AssemblyProduct::DeleteSecondaryAssembler(const std::string& name)
{
    metaAssembler.Delete(name);
}

Assembler* AssemblyProduct::GetPrimaryAssembler()
{
    CHECK_ALWAYS(metaAssembler[priAssemblerName], "Impossible.");
    return metaAssembler[priAssemblerName]; 
}

Assembler *AssemblyProduct::GetSecondaryAssembler(const std::string& name)
{
    Assembler* p = metaAssembler[name];

/// TODO Cannot do this if we use AddSecondaryAssemblerIfNotExisting()
//
///    if(!p)
///    {
///        Log::Instance().write("Invalid attempt to access assembler \"%s\"\n", name.c_str());
///    }
///
///    CHECK_ALWAYS(p, 0);
    return p;
}

