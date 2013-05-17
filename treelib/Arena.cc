
// vi: tabstop=4:expandtab

#include "Arena.hh"

#include <cstddef>

namespace treelib
{

// Align on 8 byte boundary
const int Arena::ALIGN = 8;

/** Small implementation of the die() function
 */
#define die(err)                            \
    do {                                \
        fprintf(stderr, " error at %s:%d: "             \
                err "\n", __FILE__, __LINE__);      \
        exit(128);                      \
    } while(0);

Arena::Arena(size_t size)
: size_(size), mem_((char* )malloc(size)), pos_(mem_)
{
    if(0 == mem_) {
        die("Arena out of memory.");
    }
    
    // If malloc doesn't align properly we have a problem
    if(0 != ((size_t)mem_)%ALIGN) {
        die("Wrong alignment.");
    }
}

Arena::~Arena()
{
    free(mem_);
}

void* Arena::alloc(size_t nbytes)
{
    if(pos_+nbytes - mem_ > (ptrdiff_t )size_) {
        return 0;
    }
    
    while(0 != ((size_t )pos_)%ALIGN) {
        ++pos_;
    }
    
    void* block = pos_;
    
    if(pos_+nbytes - mem_ > (ptrdiff_t )size_) {
        return 0;
    }
    
    pos_ += nbytes;
    
    return block;
}

}

