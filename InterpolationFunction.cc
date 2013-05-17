
// vi: tabstop=4:expandtab

#include <string.h>

#include "InterpolationFunction.hh"
#include "DesignByContract.hh"


// A list of points. Just for convenience ...
class PtList
{

public:
    // Create a new instance
    PtList(int n, const double* ws)
    : N(n)
    {
        CHECK_PRECOND_DEBUG(n < MAX_PTS+1);
        memcpy(vs, ws, n*sizeof(double));
    }

    // Copy constructor
    PtList(const PtList& l)
    : N(l.N)
    {
        memcpy(vs, l.vs, N*sizeof(double)); 
    }

    // Maximal number of points
    static const int MAX_PTS = 8;

    int N;
    double  vs[MAX_PTS];
};

InterpolationFunction::InterpolationFunction(double bkgVal)
: bkgVal_(bkgVal)
{
}

InterpolationFunction::InterpolationFunction(const InterpolationFunction& f)
: pts_(f.pts_), bkgVal_(f.bkgVal_)
{
    std::vector<GeoPrimitive*>::const_iterator P = f.pr_.begin();   
    for(; P != f.pr_.end(); ++P) {
        pr_.push_back((*P)->Clone());
    }   
}

InterpolationFunction::~InterpolationFunction()
{
    std::vector<GeoPrimitive*>::iterator P = pr_.begin();   
    for(; P != pr_.end(); ++P) {
        delete *P;
    }
}

InterpolationFunction& InterpolationFunction::AddNewPrimitive(const GeoPrimitive* P, int N, const double* pts)
{
     pr_.push_back(P->Clone());
    pts_.push_back(PtList(N,pts));
    
    return *this;
}

double InterpolationFunction::val(const double* p) const
{
    std::vector<GeoPrimitive*>::const_iterator P =  pr_.begin();
    std::vector<PtList>::const_iterator        S = pts_.begin();    
    for(; P != pr_.end(); ++P, ++S) {
        if((*P)->IsInside(p)) {
            return (*P)->Interpolate(S->N, S->vs, p);
        }
    }
    
    return bkgVal_;
}

InterpolationFunction* InterpolationFunction::Clone() const
{
    return new InterpolationFunction(*this);
}

