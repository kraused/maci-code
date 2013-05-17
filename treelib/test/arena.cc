
// vi: tabstop=4:expandtab

#include "treelib/treelib.hh"

// Require @expr to be true
#define require(expr) require_(expr, #expr, __FILE__, __LINE__);
void require_(bool expr, const char* str, const char* file, int line)
{
    if(!expr) {
        printf("%s failed at %s:%d\n", str, file, line);
        abort();
    }
}

int main(int argc, char** argv)
{
    treelib::Arena A(10*sizeof(int));
    
    int* block0 = (int* )A.alloc(2*sizeof(int));
    block0[0] = 5;
    block0[1] = 6;
    
    require(0 == ((size_t )block0)%treelib::Arena::ALIGN)
    
    int* block1 = (int* )A.alloc(3*sizeof(int));
    block1[0] = 3;
    block1[1] = 2;
    block1[2] = 6;
    
    require(0 == ((size_t )block1)%treelib::Arena::ALIGN)
    require(block1 - block0 >= 2);
    
    int* block2 = (int* )A.alloc(3*sizeof(int));
    block2[0] = 8;
    block2[1] = 9;
    block2[2] = 4;
    
    require(0 == ((size_t )block2)%treelib::Arena::ALIGN)
    require(block2 - block1 >= 3);
    
    int* block3 = (int* )A.alloc(1*sizeof(int));
    
    require(0 == block3);
    
}

