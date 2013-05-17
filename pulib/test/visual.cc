
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

typedef pulib::ShepardEvaluator<2,pulib::TensorBSpline<2,1>/**/>    Ev2;
typedef pulib::ShepardEvaluator<3,pulib::TensorBSpline<3,1>/**/>    Ev3;

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
        int  N   = 1000;
        double3  box = {1.,1.,1.};
    
        double3* pt2  = new double3[N];
        halton(2, N, box, pt2);

        int      dims[3];
        int      periods[3];
        MPI_Comm cart;
        
        dims[0] = 1;
        dims[1] = 1;
        dims[2] = 1;
        
        periods[0] = 0;
        periods[1] = 0;
        periods[2] = 0;
    
        MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart);

        double3* hJ2  = new double3[N];
        for(int i = 0; i < N; ++i) {
            hJ2[i][0] = 0.075;
            hJ2[i][1] = 0.075;
        }
        
        srand(0);
        
        int* idx = new int[N];
        for(int i = 0; i < N; ++i) {
            idx[i] = rand();
        }

        int nbranks[pulib::GlobalTree<2>::NBNEIGHBORS+1];
        for(int i = 0; i < pulib::GlobalTree<2>::NBNEIGHBORS+1; ++i) {
            nbranks[i] = MPI_PROC_NULL;
        }

        pulib::ApproximationSpace<2>    X(cart, nbranks, Box2(), pt2, hJ2, idx, N);
        delete[] pt2;
        
        pulib::PDVisualizer<2>  vis0("tree2.pd");       
        pulib::plotTree(X, vis0);
        
        pulib::PDVisualizer<2>  vis1("patches2.pd");        
        pulib::plotPatches(X, vis1);
        
        pulib::PDVisualizer<2>  vis2("hierarchy2.pd");      
        pulib::plotPatchHierarchy(X, vis2);
        
        pulib::PDVisualizer<2>  vis3("function2.pd");       
        pulib::plotBasisFunction<2,Ev2>(*X.getFunction(idx[N/2]), vis3);
                
        MPI_Comm_free(&cart);
    }

    {
        int  N   = 1000;
        double3  box = {1.,1.,1.};
    
        double3* pt3  = new double3[N];
        halton(3, N, box, pt3);

        int      dims[3];
        int      periods[3];
        MPI_Comm cart;

        dims[0] = 1;
        dims[1] = 1;
        dims[2] = 1;
            
        periods[0] = 0;
        periods[1] = 0;
        periods[2] = 0;
    
        MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, 1, &cart);
            
        double3* hJ3  = new double3[N];
        for(int i = 0; i < N; ++i) {
            hJ3[i][0] = 0.25;
            hJ3[i][1] = 0.25;
            hJ3[i][2] = 0.25;
        }       

        srand(0);
        
        int* idx = new int[N];
        for(int i = 0; i < N; ++i) {
            idx[i] = rand();
        }
        

        int nbranks[pulib::GlobalTree<3>::NBNEIGHBORS+1];
        for(int i = 0; i < pulib::GlobalTree<3>::NBNEIGHBORS+1; ++i) {
            nbranks[i] = MPI_PROC_NULL;
        }           
        
        pulib::ApproximationSpace<3> X(cart, nbranks, Box3(), pt3, hJ3, idx, N);
        delete[] pt3;
        
        pulib::PDVisualizer<3>  vis0("tree3.pd");       
        pulib::plotTree(X, vis0);
        
        pulib::PDVisualizer<3>  vis1("patches3.pd");        
        pulib::plotPatches(X, vis1);
        
        pulib::PDVisualizer<3>  vis2("hierarchy3.pd");      
        pulib::plotPatchHierarchy(X, vis2);
        
        pulib::PDVisualizer<3>  vis3("function3.pd");       
        pulib::plotBasisFunction<3,Ev3>(*X.getFunction(idx[N/2]), vis3);

        MPI_Comm_free(&cart);
    }
    
    return MPI_Finalize();
}

