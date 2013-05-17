
// vi: tabstop=4:expandtab

#include "treelib/treelib.hh"
#include "pulib/pulib.hh"
#include "polylib/polylib.hh"

typedef double  double3[3];

int main(int argc, char** argv)
{
    double3 lower2, upper2, lower3, upper3;
    
    lower2[0] = 0.;
    lower2[1] = 0.;
    upper2[0] = 2.;
    upper2[1] = 2.;
    pulib::Patch<2> patch2(lower2, upper2);

    lower3[0] = 0.;
    lower3[1] = 0.;
    lower3[2] = 0.;
    upper3[0] = 3.;
    upper3[1] = 1.;
    upper3[2] = 4.;
    pulib::Patch<3> patch3(lower3, upper3);
}

