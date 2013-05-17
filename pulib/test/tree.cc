
// vi: tabstop=4:expandtab

#include "treelib/treelib.hh"
#include "pulib/pulib.hh"
#include "polylib/polylib.hh"
#include "halton.hh"

#include <time.h>

// No need to include mpi.h; Either it is included in puParallel.hh
// or stubs are provided

typedef pulib::Types<2>::Box        Box2;
typedef pulib::Types<3>::Box        Box3;
typedef pulib::Types<2>::double3    double3;

// Get dimensions
void getDimensions(int ndims, int* dims)
{
    int numprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    
    dims[0] = 0;
    dims[1] = 0;
    dims[2] = 0;
    
    MPI_Dims_create(numprocs, ndims, dims);
}

// Given shifts, compute ranks in a cartesian communicator
static int CartRankFromShift(MPI_Comm cartcomm, int dim, int i, int j, int k = 0)
{
    int myrank;
    MPI_Comm_rank(cartcomm, &myrank);

    int coords[3], periods[3], dims[3];
    MPI_Cart_get(cartcomm, dim, dims, periods, coords);
    
    coords[0] += i;
    if(!periods[0] && (coords[0] < 0 || coords[0] >= dims[0])) {
        return MPI_PROC_NULL;
    }
    coords[1] += j;
    if(!periods[1] && (coords[1] < 0 || coords[1] >= dims[1])) {
        return MPI_PROC_NULL;
    }
    coords[2] += k;
    if(!periods[2] && (coords[2] < 0 || coords[2] >= dims[2])) {
        return MPI_PROC_NULL;
    }
    
    int rank;
    MPI_Cart_rank(cartcomm, coords, &rank);
    
    return rank;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int numprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    pulib::Tree<2> t2(Box2());
    pulib::Tree<3> t3(Box3());

    {   
        int      N   = 1000;
        double3  box = {1.,1.,1.};
    
        double3* pt2  = new double3[N];
        halton(2, N, box, pt2);

        int      dims[3];
        int      periods[3];
        MPI_Comm cart;
    
        getDimensions(2, dims);
    
        periods[0] = 0;
        periods[1] = 0;
        periods[2] = 0;
    
        MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart);
    
        int     myrank;
        MPI_Comm_rank  (cart, &myrank);
        int     mycoords[3];
        MPI_Cart_coords(cart, myrank, 2, mycoords);
        
        double3 lower, upper;
        for(int d = 0; d < 2; ++d) {
            lower[d] = (mycoords[d]  )/(float )dims[d];
            upper[d] = (mycoords[d]+1)/(float )dims[d];
        }
        
        for(int i = 0; i < N; ++i) {
            for(int d = 0; d < 2; ++d) {
                pt2[i][d] = (upper[d] - lower[d])*pt2[i][d] + lower[d];
            }
        }
        
        double3* hJ2  = new double3[N];
        for(int i = 0; i < N; ++i) {
            hJ2[i][0] = 0.10;
            hJ2[i][1] = 0.05;
        }       
        
        srand(0);
        
        int* idx = new int[N];
        for(int i = 0; i < N; ++i) {
            idx[i] = rand();
        }       
        
        int nbranks[pulib::GlobalTree<2>::NBNEIGHBORS+1];
        int l = 0;
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                nbranks[l++] = CartRankFromShift(cart, 2, i, j);
            }
        }       
        
        pulib::Basis <2>    S(pt2, hJ2, idx, N);        
        pulib::GlobalTree<2>    T(Box2(lower,upper), S, cart, nbranks);
        
        S.printStatistic();
        
        MPI_Comm_free(&cart);
    }

    {
        int      N   = 10000;
        double3  box = {1.,1.,1.};
    
        double3* pt3  = new double3[N];
        halton(3, N, box, pt3);

        int      dims[3];
        int      periods[3];
        MPI_Comm cart;
    
        getDimensions(3, dims);
    
        periods[0] = 0;
        periods[1] = 0;
        periods[2] = 0;
    
        MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, 1, &cart);
    
        int     myrank;
        MPI_Comm_rank  (cart, &myrank);
        int     mycoords[3];
        MPI_Cart_coords(cart, myrank, 3, mycoords);
        
        double3 lower, upper;
        for(int d = 0; d < 3; ++d) {
            lower[d] = (mycoords[d]  )/(float )dims[d];
            upper[d] = (mycoords[d]+1)/(float )dims[d];
        }
        
        for(int i = 0; i < N; ++i) {
            for(int d = 0; d < 3; ++d) {
                pt3[i][d] = (upper[d] - lower[d])*pt3[i][d] + lower[d];
            }
        }
        
        double3* hJ3  = new double3[N];
        for(int i = 0; i < N; ++i) {
            hJ3[i][0] = 0.10;
            hJ3[i][1] = 0.05;
            hJ3[i][2] = 0.07;
        }       
        
        srand(0);
        
        int* idx = new int[N];
        for(int i = 0; i < N; ++i) {
            idx[i] = rand();
        }       
        
        int nbranks[pulib::GlobalTree<3>::NBNEIGHBORS+1];
        int l = 0;
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                for(int k = -1; k < 2; ++k) {
                    nbranks[l++] = CartRankFromShift(cart, 3, i, j, k);
                }
            }
        }               
        
        pulib::Basis <3>    S(pt3, hJ3, idx, N);        
        pulib::GlobalTree<3>    T(Box3(lower,upper), S, cart, nbranks);
        
        S.printStatistic();
        
        MPI_Comm_free(&cart);
    }
    
    return MPI_Finalize();
}

