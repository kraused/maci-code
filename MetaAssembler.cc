
// vi: tabstop=4:expandtab

#include "MetaAssembler.hh"
#include "DesignByContract.hh"


int MetaAssembler::numInstances = 0;
MetaAssemblerImplementation *MetaAssembler::impl = 0;

MetaAssembler::MetaAssembler()
{
    if(0 == (numInstances++)) 
    {
        CHECK_PRECOND_ALWAYS(0 == impl);
        impl = new MetaAssemblerImplementation();
    }
}

MetaAssembler::~MetaAssembler()
{
    CHECK_PRECOND_ALWAYS(numInstances > 0);
    if(0 == (--numInstances)) 
    {
        delete impl;
        impl = 0;
    }
}

int MetaAssembler::Add(const std::string& name, Assembler *x)
{
    CHECK_PRECOND_ALWAYS(impl);
    return impl->Add(name, x);
}

void MetaAssembler::Delete(const std::string& name)
{
    CHECK_PRECOND_ALWAYS(impl);
    impl->Delete(name);
}

Assembler* MetaAssembler::operator[](const std::string& name)
{
    CHECK_PRECOND_ALWAYS(impl);
    return (*impl)[name];
}

