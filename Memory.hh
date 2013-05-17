
// vi: tabstop=4:expandtab

#ifndef MACI_MEMORY_HH_INCLUDED
#define MACI_MEMORY_HH_INCLUDED 1

#include "Pointers.hh"


/// Memory: Memory management module for the code
/// Some of the matrices in the code can grow very
/// large and this interface should allow me to 
class Memory : public Pointers
{

public:
    Memory(Maci* app);


    /// Prevent copy construction and
    /// assignments.
private:
    Memory(const Memory& other) : Pointers(other) { }
    void operator=(const Memory& other) { }
};

#endif

