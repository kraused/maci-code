
// vi: tabstop=4:expandtab

#include "Maci.hh"
#include "Memory.hh"


Maci::Maci()
{
    solver    = 0;      /// Set on script level
    system    = 0;      /// Set on script level
    procel    = 0;      /// Set on script level
    handshake = 0;      /// Set on script level
    weights   = 0;      /// set on script level

    /// TODO If we add log as member of Maci, classes
    ///      which do not inherit from Pointers will
    ///      not have access to Log (and neither will
    ///      the script level -- eventhough I don't consider
    ///      this a big problem). This is because the
    ///      codes was not initially written with this
    ///      structure in mind.
    /*
    log       = new Log(this);
    */
    memory    = new Memory(this);
}

Maci::~Maci()
{
    delete memory;
}

