
// vi: tabstop=4:expandtab

#include <stdio.h>
#include <math.h>

#include "polylib/Box.hh"
#include "quad/GaussPointList.hh"

template<typename Tp, int D, typename F>
Tp integrate(int o, const Tp* lower, const Tp* upper, const F& f)
{
    Tp I = 0.0;

    quad::GaussPointList<Tp, D> L(o, polylib::Box<Tp, D>(lower, upper));
    for(quad::GaussPoint<Tp, D> p = L.begin(); p != L.end(); ++p)
    {
        I += p.weight()*f(p.global());
    }

    return I;
}

template<typename Tp, int D>
struct Const
{
    
    Const(Tp x)
    : c(x)
    {
    }

    /// The constant
    Tp c;

    Tp operator()(const Tp* p) const
    {
        return c;
    }
};

template<typename Tp, int D>
struct Sin
{

    Tp operator()(const Tp* p) const
    {
        if(2 == D)
            return sin(M_PI*p[0])*sin(2*M_PI*p[1]);
        if(3 == D)
            return sin(M_PI*p[0])*sin(2*M_PI*p[1])*sin(2*M_PI*p[2]);

        return -1e3;
    }

};

template<typename Tp, int D>
struct Linear
{

    Tp operator()(const Tp* p) const
    {
        return p[0];
    }

};

void test2(const double* lower, const double* upper)
{
    printf(" I_[2,0](1)      = %f.\n", integrate<double, 2, Const<double, 2>/**/>(0, lower, upper, Const<double, 2>(1.0)));
    printf(" I_[2,1](1)      = %f.\n", integrate<double, 2, Const<double, 2>/**/>(1, lower, upper, Const<double, 2>(1.0)));

    printf(" I_[2,0](sin**2) = %f.\n", integrate<double, 2, Sin<double, 2>/**/>(0, lower, upper, Sin<double, 2>()));
    printf(" I_[2,1](sin**2) = %f.\n", integrate<double, 2, Sin<double, 2>/**/>(1, lower, upper, Sin<double, 2>()));

    printf(" I_[2,0](lin)    = %f.\n", integrate<double, 2, Linear<double, 2>/**/>(0, lower, upper, Linear<double, 2>()));
    printf(" I_[2,1](lin)    = %f.\n", integrate<double, 2, Linear<double, 2>/**/>(1, lower, upper, Linear<double, 2>()));
}

void test3(const double* lower, const double* upper)
{
    printf(" I_[3,0](1)      = %f.\n", integrate<double, 3, Const<double, 3>/**/>(0, lower, upper, Const<double, 3>(1.0)));
    printf(" I_[3,1](1)      = %f.\n", integrate<double, 3, Const<double, 3>/**/>(1, lower, upper, Const<double, 3>(1.0)));

    printf(" I_[3,0](sin**3) = %f.\n", integrate<double, 3, Sin<double, 3>/**/>(0, lower, upper, Sin<double, 3>()));
    printf(" I_[3,1](sin**3) = %f.\n", integrate<double, 3, Sin<double, 3>/**/>(1, lower, upper, Sin<double, 3>()));

    printf(" I_[2,0](lin)    = %f.\n", integrate<double, 3, Linear<double, 3>/**/>(0, lower, upper, Linear<double, 3>()));
    printf(" I_[2,1](lin)    = %f.\n", integrate<double, 3, Linear<double, 3>/**/>(1, lower, upper, Linear<double, 3>()));
}

int main(int argc, char** argv)
{
    double C2[4][2] = { { 0.0, 0.0 }, 
                        { 1.0, 1.0 } }; 

    test2(C2[0], C2[1]);

    double D2[4][3] = { { 0.25, 0.25 },
                        { 0.75, 0.35 } };
    
    test2(D2[0], D2[1]);

    double C3[8][3] = { { 0.0, 0.0, 0.0 },
                        { 1.0, 1.0, 1.0 } };

    test3(C3[0], C3[1]);

    double D3[8][3] = { { 0.25, 0.25, 0.25 },
                        { 0.75, 0.35, 0.55 } };
    test3(D3[0], D3[1]);

    return 0;
}

