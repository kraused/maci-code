
// vi: tabstop=4:expandtab

#include "polylib/polylib.hh"
#include "treelib/treelib.hh"
#include "halton.hh"

// Guess what ...
template<int D>
class EmptyClass : public treelib::OrthRangeTreeAttachableData<D,EmptyClass<D>/**/>
{
};

typedef treelib::Types<2>::Box  Box2;
typedef treelib::Types<3>::Box  Box3;

typedef treelib::Types<2>::double3  double3;

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
    int      N   = 10000;
    double3  box = {1.,1.,1.};
    
    double3* h2  = new double3[N];
    halton(2, N, box, h2);
    double3* h3  = new double3[N];
    halton(3, N, box, h3);

    EmptyClass<2>   instance2;
    EmptyClass<3>   instance3;

    double lower[] = { 0., 0., 0. };
    double upper[] = { 1., 1., 1. };

    Box2 box2(lower,upper);
    Box3 box3(lower,upper);
    
    treelib::OrthRangeTree<2,EmptyClass<2>/**/> T2(box2);
    for(int i = 0; i < N; ++i) {
        T2.insert(h2[i], instance2);
    }
    
    treelib::OrthRangeTree<3,EmptyClass<3>/**/> T3(box3);
    for(int i = 0; i < N; ++i) {
        T3.insert(h3[i], instance3);
    }
    
    require(N == T2.getNumLeaves());
    require(N == T3.getNumLeaves());
}

