
// vi: tabstop=4:expandtab

#include "PluginTable.hh"


PluginTable::PluginTable(Maci* app)
: Pointers(app)
{
}

PluginTable::~PluginTable()
{
    for(std::size_t i = 0; i < size(); ++i)
        delete (*this)[i];
}

void PluginTable::insert_plugin(const Plugin& p)
{
    push_back(p.clone());
}

void PluginTable::setup()
{
    for(BaseVector::iterator it = begin(); it != end(); ++it)
        (*it)->setup();
}

void PluginTable::pre_integration()
{
    for(BaseVector::iterator it = begin(); it != end(); ++it)
        (*it)->pre_integration();
}

void PluginTable::post_integration()
{
    for(BaseVector::iterator it = begin(); it != end(); ++it)
        (*it)->post_integration();
}

void PluginTable::pre_force()
{
    for(BaseVector::iterator it = begin(); it != end(); ++it)
        (*it)->pre_force();
}

void PluginTable::post_force()
{
    for(BaseVector::iterator it = begin(); it != end(); ++it)
        (*it)->post_force();
}

void PluginTable::pre_step()
{
    for(BaseVector::iterator it = begin(); it != end(); ++it)
        (*it)->pre_step();
}

void PluginTable::post_step()
{
    for(BaseVector::iterator it = begin(); it != end(); ++it)
        (*it)->post_step();
}

