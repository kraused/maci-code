
// vi: tabstop=4:expandtab

#include "Plugin.hh"
#include "exception/Throw.hh"
#include "DesignByContract.hh"
#include "ProcElement.hh"
#include "Maci.hh"
#include "Logging.hh"


Plugin::Plugin(Maci* app, const std::string& name)
: Pointers(app)
{
    Log::Instance().write("Added Plugin \"%s\".\n", name.c_str());
}

