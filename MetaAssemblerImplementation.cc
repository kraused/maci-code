
// vi: tabstop=4:expandtab

#include "MetaAssemblerImplementation.hh"


MetaAssemblerImplementation::~MetaAssemblerImplementation()
{
    for(std::map<std::string,Assembler*>::iterator i = assembler.begin(); i != assembler.end(); ++i)
        delete i->second;
}

int MetaAssemblerImplementation::Add(const std::string& name, Assembler *x)
{
    int retval;
    std::map<std::string,Assembler*>::iterator i = assembler.find(name);

    if(assembler.end() == i)
    {
        assembler[name] = x;
        retval = 0;
    }
    else
        retval = 1;

    return retval;
}

void MetaAssemblerImplementation::Delete(const std::string& name)
{
    std::map<std::string,Assembler*>::iterator i = assembler.find(name);
    CHECK_ALWAYS(assembler.end() != i, "Could not find assembler: Nothing to delete");

    delete i->second;
    assembler.erase(i);
}

Assembler *MetaAssemblerImplementation::operator[](const std::string& name)
{
    std::map<std::string,Assembler*>::iterator i = assembler.find(name);
    if(assembler.end() == i)
        return NULL;
    else
        return assembler[name];
}

