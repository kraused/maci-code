
// vi: tabstop=4:expandtab

#ifndef PULIB_VISUALIZE_HH_INCLUDED
#define PULIB_VISUALIZE_HH_INCLUDED 1

#include "puTree.hh"
#include "puBasisFunction.hh"
#include "pulib/typedefs.hh"
#include "puSpace.hh"

namespace pulib
{

// The base class for all visualizer
template<int D>
class Visualizer
{

public:
    // Construt a new visualizer; @name is the name of
    // the output file
    Visualizer(const char name[]);
    
    // Virtual destructor to keep the compiler quiet
    virtual ~Visualizer()
    {
    }

    // Plot a point @x
    virtual void plotPoint(const double x[]) = 0;

    // Plot a point @x and an associated value @val
    virtual void plotPoint(const double x[], double val) = 0;

    // Plot a box &box
    virtual void plotBox(const typename Types<D>::Box& box) = 0;

    // Plot a box @box and corner associated values
    virtual void plotBox(const typename Types<D>::Box& box, double val[]) = 0;

    // Plot a line given by @start and @end
    virtual void plotLine(const double start[], const double end[]) = 0;

    // Plot a line given by @start and @end; @val are corner associated values (scalar)
    virtual void plotLine(const double start[], const double end[], double val[]) = 0;

private:
    // Maximal length of output file
    static const int    MAX_NAMELENGTH = 128;
    // Name of output file
    char                name_[MAX_NAMELENGTH];
};

// Visual representation of the PU tree (boxes and points)
template<int D>
void plotTree(ApproximationSpace<D>& X, Visualizer<D>& vis);

// Plot patches+particles
template<int D>
void plotPatches(ApproximationSpace<D>& X, Visualizer<D>& vis);

// Plot patches+particles and superpatches
template<int D>
void plotPatchHierarchy(ApproximationSpace<D>& X, Visualizer<D>& vis);

// Plot a generating function
// @Ev is the evaluator class
template<int D, typename Ev>
void plotBasisFunction(const BasisFunction<D>& phi, Visualizer<D>& vis);


// An implementation of the @Visualizer interface
// This class allows to write .pd files which can be converted
// to vtk files
template<int D>
class PDVisualizer : public Visualizer<D>
{

public:
    // Create a new PD Visualizer instance, opens a new file
    PDVisualizer(const char name[]);
    
    // Destroy the instance. This function closes the file
    ~PDVisualizer();

    // virtual; overrides pure virtual function in base class
    void plotPoint(const double x[]);

    // virtual; overrides pure virtual function in base class
    void plotPoint(const double x[], double val);

    // virtual; overrides pure virtual function in base class
    void plotBox(const typename Types<D>::Box& box);

    // virtual; overrides pure virtual function in base class
    void plotBox(const typename Types<D>::Box& box, double val[]);

    // virtual; overrides pure virtual function in base class
    void plotLine(const double start[], const double end[]);

    // virtual; overrides pure virtual function in base class
    void plotLine(const double start[], const double end[], double val[]);
    
private:
    // The output file
    FILE*   fh_;
};



template<int D>
Visualizer<D>::Visualizer(const char name[])
{
    snprintf(name_, MAX_NAMELENGTH, name);
}


// Plot @phi on the edges of @box
template<int D, typename Ev>
void plotBasisFunctionOnEdges(const BasisFunction<D>& phi, const typename Types<D>::Box box, Visualizer<D>& vis)
{
    int nedges = (2 == D) ? 4 : 12;

    static const int edges[][2] = { {0, 1}, {1, 2}, {2, 3}, {3, 0},
                                    {4, 5}, {5, 6}, {6, 7}, {7, 4},
                                    {0, 4}, {1, 5}, {2, 6}, {3, 7} };
    static const int dir[]      = { 0, 1, 0, 1, 0, 1, 0, 1, 2, 2, 2, 2 };
    static const int npts[]     = { 20, 20, 20 };   
    
    double dx, x, start[3], end[3], val[3];
    
    for(int i = 0; i < nedges; ++i) {
        dx = 1./(float )npts[dir[i]];
        for(int j = 0; j < npts[dir[i]]; ++j) {
            x = j*dx;
            
            for(int d = 0; d < D; ++d) {
                start[d] = (1 - x   )*box.getCorner(edges[i][0])[d] +  x    *box.getCorner(edges[i][1])[d];
                end[d]   = (1 - x-dx)*box.getCorner(edges[i][0])[d] + (x+dx)*box.getCorner(edges[i][1])[d];
            }
            
            val[0] = Ev::value(phi, start);
            val[1] = Ev::value(phi, end);
            
            vis.plotLine(start, end, val);
        }
    }
}

// Plot @phi in the interior of @box
template<int D, typename Ev>
void plotBasisFunctionInInterior(const BasisFunction<D>& phi, const typename Types<D>::Box box, Visualizer<D>& vis)
{
    int     npts[] = { 20, 20, ((D == 2) ? 1 : 20) }, ijk[3];
    double  pt[3];
    
    for(ijk[0] = 0; ijk[0] < npts[0]; ++ijk[0]) {
        for(ijk[1] = 0; ijk[1] < npts[1]; ++ijk[1]) {
            for(ijk[2] = 0; ijk[2] < npts[2]; ++ijk[2]) {
                
                for(int d = 0; d < D; ++d) {
                    pt[d] = box.getLower()[d] + box.getSidelength(d)*ijk[d]/(double )(npts[d]-1);
                }
                
                vis.plotPoint(pt, Ev::value(phi, pt));
            }
        }
    }
}

template<int D, typename Ev>
void plotBasisFunction(const BasisFunction<D>& phi, Visualizer<D>& vis)
{
    int                     ncells;
    typename Types<D>::Box* cells;
    
    computeQuadCells(phi, &ncells, &cells);
    
    for(int i = 0; i < ncells; ++i) {
        plotBasisFunctionOnEdges<D,Ev>   (phi, cells[i], vis);
        plotBasisFunctionInInterior<D,Ev>(phi, cells[i], vis);
    }
    
    delete[] cells;
}


template<int D>
PDVisualizer<D>::PDVisualizer(const char name[])
: Visualizer<D>(name), fh_(fopen(name, "w"))
{
}

template<int D>
PDVisualizer<D>::~PDVisualizer()
{
    if(fh_) fclose(fh_);
}

template<int D>
void PDVisualizer<D>::plotPoint(const double x[])
{
    fprintf(fh_, "POINT %16.15f %16.15f %16.15f\n", x[0], x[1], ((D == 2) ? 0. : x[2]));
}

template<int D>
void PDVisualizer<D>::plotPoint(const double x[], double val)
{
    fprintf(fh_, "POINT %16.15f %16.15f %16.15f %16.15f\n", x[0], x[1], ((D == 2) ? val : x[2]), val);
}

template<int D>
void PDVisualizer<D>::plotBox(const typename Types<D>::Box& box)
{
    if(2 == D) {
        fprintf(fh_, "QUAD ");
    } else {
        fprintf(fh_, "HEX ");   
    }

    for (int i = 0; i < (1<<D); ++i) {
        fprintf(fh_, "%16.15f %16.15f %16.15f ",
                box.getCorner(i)[0], box.getCorner(i)[1], ((D == 2) ? 0. : box.getCorner(i)[2]));
    }
    
    fprintf(fh_, "\n");
}

template<int D>
void PDVisualizer<D>::plotBox(const typename Types<D>::Box& box, double val[])
{   
    if(2 == D) {
        fprintf(fh_, "QUAD ");
    } else {
        fprintf(fh_, "HEX ");   
    }

    // Use averaged value
    double avg = 0;

    for (int i = 0; i < (1<<D); ++i) {
        fprintf(fh_, "%16.15f %16.15f %16.15f ",
                box.getCorner(i)[0], box.getCorner(i)[1], ((D == 2) ? 0. : box.getCorner(i)[2]));
                    
        avg += val[i];
    }
    
    avg /= (double )(1<<D);

    fprintf(fh_, "%16.15f\n", avg);
}

template<int D>
void PDVisualizer<D>::plotLine(const double start[], const double end[])
{
    fprintf(fh_, "LINE %16.15f %16.15f %16.15f %16.15f %16.15f %16.15f\n",
            start[0], start[1], ((2 == D) ? 0. : start[2]), end[0], end[1], ((2 == D) ? 0. : end[2]));
}

template<int D>
void PDVisualizer<D>::plotLine(const double start[], const double end[], double val[])
{
    // Use averaged value
    double avg = 0.5*(val[0] + val[1]);

    fprintf(fh_, "LINE %16.15f %16.15f %16.15f %16.15f %16.15f %16.15f %16.15f\n",
            start[0], start[1], ((2 == D) ? val[0] : start[2]), end[0], end[1], ((2 == D) ? val[1] : end[2]), avg);
}

}

#endif 

