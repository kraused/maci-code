
// vi: tabstop=4:expandtab

#ifndef TREELIB_TEST_HALTON_HH_INCLUDED
#define TREELIB_TEST_HALTON_HH_INCLUDED    1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef double double3[3];

static inline
void halton(int dim, int npts, double3 box, double3* h)
{
    double  lognpts = 0., x = 0.;
    int     n = 0;
    static  int P[] = {2,3,5};
    
    lognpts = log((double )npts+1.);
    
    for(int d = 0; d < dim; ++d) {
        n = (int )ceil((double )lognpts/(double )(log((double )P[d])));
        
        for(int j = 0; j < npts; ++j) {
            x = (double )(j+1);
            
            h[j][d] = fmod(x,(double )P[d])/(double )P[d];
            for(int i = 1; i < n; ++i) {
                x        = floor(x/(double )P[d]);
                h[j][d] += fmod(x,(double )P[d])*pow((double )P[d],-(i+1));
            }
        }
    }
    
    for(int i = 0; i < npts; ++i) {
        for(int d = 0; d < dim; ++d) {
            h[i][d] *= box[d];
        }
    }
}


void printHalton()
{
    /* Number of points */
    int const N  = 1000000;
    
    FILE*    f   = NULL;
    double3* h   = (double3* )calloc(N,sizeof(double3));
    double3  box = {1.,1.,1.};
    
    f = fopen("halton2.data","w");
    halton(2, N, box, h);
    for(int i = 0; i < N; ++i) {
        fprintf(f,"%16.15f\t%16.15f\t%16.15f\n", h[i][0], h[i][1], h[i][2]);
    }
    fclose(f);

    f = fopen("halton3.data","w");
    halton(3, N, box, h);
    for(int i = 0; i < N; ++i) {
        fprintf(f,"%16.15f\t%16.15f\t%16.15f\n", h[i][0], h[i][1], h[i][2]);
    }
    fclose(f);
}

#endif

