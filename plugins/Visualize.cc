
// vi: tabstop=4:expandtab

#include "Visualize.hh"
#include "DesignByContract.hh"
#include "ProcElement.hh"
#include "Maci.hh"
#include "Logging.hh"

#include "utils/strprintf.hh"

using namespace plugins;

/* -----------------------------------------------------------------------------
 * Class member functions
 * -------------------------------------------------------------------------- */

Visualize::Visualize(Maci* app, int themask, const char *theprefix)
: Plugin(app, "Visualize"), mask(themask), step(0), prefix(theprefix)
{
    // Note: The suffix argument was removed because the underlying
    //       I/O library is assumed to provide the correct suffix. This
    //       is necessary because in general we will not only write a
    //       single file but also metadata files, or data could be split
    //       into multiple files ...
}

Visualize::Visualize(const Visualize &o)
: Plugin(o), mask(o.mask), step(o.step), prefix(o.prefix)
{
}

Visualize* Visualize::clone() const
{
    return new Visualize(*this);
}

void Visualize::setup()
{
    // Write reference situation
    write(prefix + ".000000");
}

void Visualize::post_step()
{
    ++step;
    if(0 == step % mask)
        write(utils::strprintf("%s.%06d", prefix.c_str(), step));
}

void Visualize::write(const std::string& filename)
{
    Log::Instance().write("File is open: \"%s\".\n", filename.c_str());

    switch(procel->mytype) 
    {
        case PROC_MD:
            system->Write(filename);
            break;
        case PROC_FE:
            solver->Write(filename);
            break;
        default:
            THROW(BadDefaultException, "");
    }
}

