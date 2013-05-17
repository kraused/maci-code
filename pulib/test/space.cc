
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

template<int D>
static void printBox(FILE* fh, const polylib::Box<D>& B)
{
    if(2 == D) {
        fprintf(fh, "QUAD ");

        for (int i = 0; i < 4; ++i) {
                fprintf(fh, "%16.15f %16.15f %16.15f ",
                        B.getCorner(i)[0], B.getCorner(i)[1], 0.);
        }
        
        fprintf(fh, "\n");
    }
    if(3 == D) {
        fprintf(fh, "HEX ");

        for (int i = 0; i < 8; ++i) {
                fprintf(fh, "%16.15f %16.15f %16.15f ",
                        B.getCorner(i)[0], B.getCorner(i)[1], B.getCorner(i)[2]);
        }
        
        fprintf(fh, "\n");  
    }
}

static void printPoint2(FILE* fh, const double x[])
{
    fprintf(fh, "POINT %16.15f %16.15f 0.\n", x[0], x[1]);
}

static void printPoint3(FILE* fh, const double x[])
{
    fprintf(fh, "POINT %16.15f %16.15f %16.15f\n", x[0], x[1], x[2]);
}

#define require(expr)                       \
    if(!(expr)) {                       \
        printf("require(%s) failed ...\n", #expr);  \
        abort();                    \
    }

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int numprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    if(1 < numprocs) {
        printf(" Error: This is a serial test!\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    {   
        int  N   = 10000;
        double3  box = {1.,1.,1.};
    
        double3* pt2  = new double3[N];
        halton(2, N, box, pt2);

        int  dims[] = { 0, 0, 0 };
        int periods[3];
        MPI_Comm cart;
        
        int numprocs;
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Dims_create(numprocs, 3, dims);
        
        periods[0] = 0;
        periods[1] = 0;
        periods[2] = 0;
    
        MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart);

        double3* hJ2  = new double3[N];
        for(int i = 0; i < N; ++i) {
            hJ2[i][0] = 0.05;
            hJ2[i][1] = 0.05;
        }

        srand(0);
        
        int* idx = new int[N];
        for(int i = 0; i < N; ++i) {
            idx[i] = rand();
        }

        // Of no interest in the two serial case
        int nbranks[pulib::GlobalTree<2>::NBNEIGHBORS+1];
        for(int i = 0; i < pulib::GlobalTree<2>::NBNEIGHBORS+1; ++i) {
            nbranks[i] = MPI_PROC_NULL;
        }

        pulib::ApproximationSpace<2>    X(cart, nbranks, Box2(), pt2, hJ2, idx, N);     
        delete[] pt2;
        
        for(pulib::ApproximationSpace<2>::Iterator it = X.begin(); it != X.end(); ++it)
        {
        }
        
        for(int i = 0; i < N; ++i) {
            try {
                require(idx[i] == X.getFunction(idx[i])->getId());
            } catch(int e) {
                abort();
            }
        }
        
        try {
            X.getFunction(RAND_MAX + 1);
            abort();
        } catch(int e) {
            // It works!
        }
        
        MPI_Comm_free(&cart);
    }

    {
        int  N   = 10000;
        double3  box = {1.,1.,1.};
    
        double3* pt3  = new double3[N];
        halton(3, N, box, pt3);

        FILE* fhpt3 = fopen("pt3.pd", "w");
        for(int i = 0; i < N; ++i) {
            printPoint3(fhpt3, pt3[i]);
        }
        fclose(fhpt3);

        int dims[] = { 0, 0, 0 };
        int periods[3];
        MPI_Comm cart;
        
        int numprocs;
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Dims_create(numprocs, 3, dims);
        
        periods[0] = 0;
        periods[1] = 0;
        periods[2] = 0;
    
        MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, 1, &cart);
            
        double3* hJ3  = new double3[N];
        for(int i = 0; i < N; ++i) {
            hJ3[i][0] = 0.05;
            hJ3[i][1] = 0.05;
            hJ3[i][2] = 0.05;
        }       

        srand(0);
        
        int* idx = new int[N];
        for(int i = 0; i < N; ++i) {
            idx[i] = rand();
        }
        
        // Of no interest in the two serial case
        int nbranks[pulib::GlobalTree<3>::NBNEIGHBORS+1];
        for(int i = 0; i < pulib::GlobalTree<3>::NBNEIGHBORS+1; ++i) {
            nbranks[i] = MPI_PROC_NULL;
        }       
        
        pulib::ApproximationSpace<3> X(cart, nbranks, Box3(), pt3, hJ3, idx, N);        
        delete[] pt3;
        
        for(pulib::ApproximationSpace<3>::Iterator it = X.begin(); it != X.end(); ++it)
        {
        }

        for(int i = 0; i < N; ++i) {
            try {
                require(idx[i] == X.getFunction(idx[i])->getId());
            } catch(int e) {
                abort();
            }
        }
        
        try {
            X.getFunction(RAND_MAX + 1);
            abort();
        } catch(int e) {
            // It works!
        }

        MPI_Comm_free(&cart);
    }
    
    return MPI_Finalize();
}

