
// vi: tabstop=4:expandtab

#ifndef TREELIB_ARENA_HH_INCLUDED
#define TREELIB_ARENA_HH_INCLUDED

#include <cstdlib>
#include <cstdio>

namespace treelib
{

// A memory managing arena 
class Arena
{
    
public:
    // Construct a new arena of size @size
    Arena(size_t size);
    
    // Destroy the arena and everything allocated
    // inside it
    ~Arena();
    
    // Allocate a new block of @nbytes bytes. All
    // blocks are 
    void* alloc(size_t nbytes);
    
private:
    // Size of the arena
    size_t              size_;
    // Memory
    char*               mem_;
    // Pointer to the first free byte
    char*               pos_;
    
public:
    // Aligment
    static const int    ALIGN;
};

}

#endif

