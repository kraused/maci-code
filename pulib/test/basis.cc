
// vi: tabstop=4:expandtab

#include "treelib/treelib.hh"
#include "pulib/pulib.hh"
#include "polylib/polylib.hh"
#include "halton.hh"

#include <time.h>


typedef double  double3[3];

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
    {
        double3 pt2, hJ2, pt3, hJ3;
        
        pt2[0] = 0.;
        pt2[1] = 0.;
        hJ2[0] = 1.;
        hJ2[1] = 2.;
        pulib::BasisFunction<2> f2(pt2, hJ2, 0);

        require(0 == f2.getId());

        pt3[0] = 0.;
        pt3[1] = 0.;
        pt3[2] = 0.;
        hJ3[0] = 3.;
        hJ3[1] = 1.;
        hJ3[2] = 4.;
        pulib::BasisFunction<2> f3(pt3, hJ3, 5);

        require(5 == f3.getId());
    }
    
    {
        int      N   = 2;
        double3  box = {1.,1.,1.};
        
        double3* pt2  = new double3[N];
        halton(2, N, box, pt2);
        double3* pt3  = new double3[N];
        halton(3, N, box, pt3);
        
        double3* hJ2  = new double3[N];
        for(int i = 0; i < N; ++i) {
            hJ2[i][0] = 0.2 + ( (0 == i%10) ? 0.4 : -0.1 );
            hJ2[i][1] = 0.3;
        }
        
        double3* hJ3  = new double3[N];
        for(int i = 0; i < N; ++i) {
            hJ3[i][0] = 0.2;
            hJ3[i][1] = 0.3 + ( (0 == i%10) ? 0.2 : -0.1 );
            hJ3[i][2] = 0.1;
        }
        
        srand(0);
        
        int*    idx2  = new int[N];
        for(int i = 0; i < N; ++i) {
            idx2[i] = rand();
        }
        
        int*    idx3  = new int[N];
        for(int i = 0; i < N; ++i) {
            idx3[i] = rand();
        }
        
        pulib::Basis<2> S2(pt2, hJ2, idx2, N);
        require(S2.size() == N);
        
        int cnt2 = 0;
        for(pulib::Basis<2>::ConstIterator it = S2.begin(); it != S2.end(); ++it) {
            ++cnt2;
        }
        require(cnt2 == N);
        
        double3 maxhJ2;
        S2.getMaxPatchSidelength(maxhJ2);
        require(fabs(0.6 - maxhJ2[0]) < 1e-7);
        require(fabs(0.3 - maxhJ2[1]) < 1e-7);
        
        pulib::Basis<3> S3(pt3, hJ3, idx3, N);
        
        require(S3.size() == N);
        
        int cnt3 = 0;
        for(pulib::Basis<3>::ConstIterator it = S3.begin(); it != S3.end(); ++it) {
            ++cnt3;
        }
        require(cnt3 == N);
        
        double3 maxhJ3;
        S3.getMaxPatchSidelength(maxhJ3);
        require(fabs(0.2 - maxhJ3[0]) < 1e-7);
        require(fabs(0.5 - maxhJ3[1]) < 1e-7);
        require(fabs(0.1 - maxhJ3[2]) < 1e-7);
    }
}

