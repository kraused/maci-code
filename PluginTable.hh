
// vi: tabstop=4:expandtab

#ifndef MACI_PLUGIN_TABLE_HH_INCLUDED
#define MACI_PLUGIN_TABLE_HH_INCLUDED 1

#include <vector>
#include <stdlib.h>

#include "Plugin.hh"
#include "Pointers.hh"


/// PluginTable: A table of plugins
/// The table holds all plugins ordered and allows for starting, stoping and
/// applying plugins collectively
class PluginTable : protected std::vector<Plugin*>, public Pointers
{
    typedef std::vector<Plugin*> BaseVector;

public:
    PluginTable(Maci* app);

    /// Destructor
    ~PluginTable();

    /// Add a new plugin to the table
    void insert_plugin(const Plugin& p);

    /// Call the corresponding function for all
    /// plugins in the table
    void setup();
    void pre_integration();
    void post_integration();
    void pre_force();
    void post_force();
    void pre_step();
    void post_step();
};

#endif

