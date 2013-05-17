
// vi: tabstop=4:expandtab

#include "treelib/treelib.hh"
#include "pulib/pulib.hh"
#include "polylib/polylib.hh"
#include "halton.hh"

// No need to include mpi.h; Either it is included in puParallel.hh
// or stubs are provided

typedef pulib::Types<2>::Box        Box2;
typedef pulib::Types<3>::Box        Box3;
typedef pulib::Types<2>::double3    double3;

typedef pulib::ShepardEvaluator<2,pulib::TensorBSpline<2,1>/**/>    Evaluator2;
typedef pulib::ShepardEvaluator<3,pulib::TensorBSpline<3,1>/**/>    Evaluator3;

static void printPoint2(FILE* fh, const double x[], double val)
{
    fprintf(fh, "POINT %16.15f %16.15f %16.15f %16.15f\n", x[0], x[1], val, val);
}

static void printPoint3(FILE* fh, const double x[], double val)
{
    fprintf(fh, "POINT %16.15f %16.15f %16.15f %16.15f\n", x[0], x[1], x[2], val);
}

static double frankeFunction2(const double x[])
{
    return 0.75*exp(-((9*x[0] - 2)*(9*x[0] - 2) +
                      (9*x[1] - 2)*(9*x[1] - 2))/4);
}

static double frankeFunction3(const double x[])
{
    return 0.75*exp(-((9*x[0] - 2)*(9*x[0] - 2) +
                      (9*x[1] - 2)*(9*x[1] - 2) +
                      (9*x[2] - 2)*(9*x[2] - 2))/4);
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
        int      N   = 1000;
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
                
        FILE* fh = fopen("coarse2.pd", "w");
        for(pulib::ApproximationSpace<2>::Iterator it = X.begin(); it != X.end(); ++it) {
            printPoint2(fh, it->getParticlePt(), frankeFunction2(it->getParticlePt()));
        }
        fclose(fh);
        
        FILE*            gh = fopen("fine2.pd", "w");
        static const int n2 = 100;
        for(int i = 0; i < n2; ++i) {
            for(int j = 0; j < n2; ++j) {
                double x[] = { i/(float )n2, j/(float )n2 };
                
                double val = 0.;
                for(pulib::ApproximationSpace<2>::Iterator it = X.begin(); it != X.end(); ++it) {
                    val += frankeFunction2(it->getParticlePt())*Evaluator2::value(*it, x);
                } 
                
                printPoint2(gh, x, val);
            }
        }
        fclose(gh);
        
        MPI_Comm_free(&cart);
    }

    {
        int      N   = 1000;
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
        
        FILE* fh = fopen("coarse3.pd", "w");
        for(pulib::ApproximationSpace<3>::Iterator it = X.begin(); it != X.end(); ++it) {
            printPoint3(fh, it->getParticlePt(), frankeFunction3(it->getParticlePt()));
        }
        fclose(fh);
        
        FILE*            gh = fopen("fine3.pd", "w");
        static const int n3 = 20;
        for(int i = 0; i < n3; ++i) {
            for(int j = 0; j < n3; ++j) {
                for(int k = 0; k < n3; ++k) {
                    double x[] = { i/(float )n3, j/(float )n3, k/(float )n3 };
                
                    double val = 0.;
                    for(pulib::ApproximationSpace<3>::Iterator it = X.begin(); it != X.end(); ++it) {
                        val += frankeFunction3(it->getParticlePt())*Evaluator3::value(*it, x);
                    }
                    
                    printPoint3(gh, x, val);
                }
            }
        }
        fclose(gh);
        
        MPI_Comm_free(&cart);
    }
    
    return MPI_Finalize();
}

