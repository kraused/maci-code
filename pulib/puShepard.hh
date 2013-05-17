
// vi: tabstop=4:expandtab

#ifndef PULIB_SHEPARD_HH_INCLUDED
#define PULIB_SHEPARD_HH_INCLUDED 1

#include "puBasisFunction.hh"
#include "pulib/typedefs.hh"

#include <math.h>

namespace pulib
{

// Shepard evaluator class; @D is the dimension and @W
// is a functor defining the weight function, it must
// implement the function
// - static double value(const typename Types<D>::Box& box, const double xg[])
// which takes the point @x in global coordinates. 
// It is guaranteed that @xg is in @box!
template<int D, typename W>
class ShepardEvaluator
{
    typedef typename Types<D>::double3  double3;

public:
    // Evaluate the generating function @phiJ at the point @x
    static double value(const BasisFunction<D>& phiJ, const double xg[]);

private:
    // The treshold for the denominator
    static const double DENOM_TRESHOLD;
};

// Tensor product of B splines of order @N
template<int D, int N>
class TensorBSpline
{
    typedef typename Types<D>::double3  double3;

public:
    // Evaluate the tensor B spline
    static double value(const typename Types<D>::Box& box, const double xg[]);

private:
    // The one dimensional B spline on the interval [0,1]
    inline static double spline1D(double x);
};



template<int D, typename W>
const double ShepardEvaluator<D,W>::DENOM_TRESHOLD = 1e-10;

// Evaluate the weighting
template<int D, typename W>
inline double evalWeighting(const typename Types<D>::Box& box, const double xg[])
{
    if(box.isInside(xg,1e-12)) {
        return W::value(box, xg);
    } else {
        return 0.;
    }
}

template<int D, typename W>
double ShepardEvaluator<D,W>::value(const BasisFunction<D>& phiJ, const double xg[])
{
    double   nom = evalWeighting<D,W>(phiJ, xg);    
    double denom = 0.;

    for(typename BasisFunction<D>::OvIterator it = phiJ.begin(); it != phiJ.end(); ++it) {
        denom += evalWeighting<D,W>(*it, xg);
    }
    
    if(fabs(denom) < ShepardEvaluator<D,W>::DENOM_TRESHOLD) {
        // We always have nom < denom
        // A very small denominator means that we are close to the boundary of all
        // the containing boxes
        // In this case 1. is a good approximation to the value since the critical
        // interface is approximately low-dimensional
        return 1.;
    }
    
    ASSERT(nom > -1e-10);
    ASSERT(nom < denom + 1e-10);
        
    return (nom/denom);
}

template<int D, int N>
double TensorBSpline<D,N>::value(const typename Types<D>::Box& box, const double xg[])
{
    double3 xl;
    for(int d = 0; d < D; ++d) {
        xl[d] = (xg[d] - box.getLower()[d])/box.getSidelength(d);
    }
    
    double  val = 1.;
    for(int d = 0; d < D; ++d) {
        val *= TensorBSpline<D,N>::spline1D(xl[d]);
    }
    
    return val;
}

// B spline function on [0,1]
template<int D>
inline double BSpline(double x)
{
    return 0.;
}

template<>
inline double BSpline<0>(double x)
{
    return 1.;
}

template<>
inline double BSpline<1>(double x)
{
    // In principle I don't need the max, but accumulated errors
    // might result in an @x value which lies outside [0,1].
    return /*std::max(0.,*/ ( (x < 0.5) ? 2*x : 2*(1-x) )/*)*/;
}

template<int D, int N>
inline double TensorBSpline<D,N>::spline1D(double x)
{
    return BSpline<N>(x);
}

}

#endif

