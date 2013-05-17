
// vi: tabstop=4:expandtab

#ifndef MACI_QUAD_GAUSS_POINT_LIST_HH_INCLUDED
#define MACI_QUAD_GAUSS_POINT_LIST_HH_INCLUDED 1

/// Needed for error messages
#include <stdio.h>

#include "polylib/Box.hh"

#include "quad/GaussPointPayload.hh"
#include "quad/GaussPoint.hh"

namespace quad
{

    /// GaussPointList: A list of quadrature Gauss point
    ///
    /// The function provides access to the quadrature points
    /// for polytopes
    ///
    template<typename Tp, int D>
    class GaussPointList
    {

    public:
        /// Create a new instance of GaussPointList from an
        /// inptu box
        /// @param[in]  o   the requested order
        /// @param[in]  b   input box
        GaussPointList(int o, const polylib::Box<Tp, D>& B)
        {
            static const double PT_1D[] = { 0.211324865405187, 0.788675134594813 };

            /// --------------------------------------------------
            /// Mid point quadrature for the cube is trivial
            /// --------------------------------------------------
            if(0 == o)
            {
                npts     = 1;
                P[0].wgt = B.getVolume();

                for(int d = 0; d < D; ++d)
                    P[0].lpt[d] = 0.5;
            }
            else
            /// --------------------------------------------------
            /// First order quadrature
            /// --------------------------------------------------
            if(1 == o)
            {
                switch(D)
                {
                    case 2:
                            npts = 4;
                            
                            for(int i = 0; i < 4; ++i)
                                P[i].wgt = B.getVolume()*0.25;

                            P[0].lpt[0] = PT_1D[0];
                            P[0].lpt[1] = PT_1D[0];

                            P[1].lpt[0] = PT_1D[1];
                            P[1].lpt[1] = PT_1D[0];

                            P[2].lpt[0] = PT_1D[1];
                            P[2].lpt[1] = PT_1D[1];

                            P[3].lpt[0] = PT_1D[0];
                            P[3].lpt[1] = PT_1D[1];
                                                        
                            break;
                    case 3:
                            npts = 8;

                            for(int i = 0; i < 8; ++i)
                                P[i].wgt = B.getVolume()*0.125;

                            P[0].lpt[0] = PT_1D[0];
                            P[0].lpt[1] = PT_1D[0];
                            P[0].lpt[2] = PT_1D[0];

                            P[1].lpt[0] = PT_1D[1];
                            P[1].lpt[1] = PT_1D[0];
                            P[1].lpt[2] = PT_1D[0];

                            P[2].lpt[0] = PT_1D[1];
                            P[2].lpt[1] = PT_1D[1];
                            P[2].lpt[2] = PT_1D[0];

                            P[3].lpt[0] = PT_1D[0];
                            P[3].lpt[1] = PT_1D[1];
                            P[3].lpt[2] = PT_1D[0];

                            P[4].lpt[0] = PT_1D[0];
                            P[4].lpt[1] = PT_1D[0];
                            P[4].lpt[2] = PT_1D[1];

                            P[5].lpt[0] = PT_1D[1];
                            P[5].lpt[1] = PT_1D[0];
                            P[5].lpt[2] = PT_1D[1];

                            P[6].lpt[0] = PT_1D[1];
                            P[6].lpt[1] = PT_1D[1];
                            P[6].lpt[2] = PT_1D[1];

                            P[7].lpt[0] = PT_1D[0];
                            P[7].lpt[1] = PT_1D[1];
                            P[7].lpt[2] = PT_1D[1];
                                                        
                            break;
                    default:
                        fprintf(stderr, "Unsupported dimension D = %d.", D);
                        exit(128);
                }
            }
            else
            {
                fprintf(stderr, "Unsupported quadrature order o = %d.", o);
                exit(128);
            }

            for(int i = 0; i < npts; ++i)
                for(int d = 0; d < D; ++d)
                    P[i].gpt[d] = B.getLower()[d] + P[i].lpt[d]*B.getSidelength(d);
        }

        /// Return the beginning of the list of Gauss points
        inline GaussPoint<Tp, D> begin() const
        {
            return GaussPoint<Tp, D>(&P[0]);
        }

        /// Return the end of the list of Gauss points
        inline GaussPoint<Tp, D> end() const
        {
            return GaussPoint<Tp, D>(&P[npts]);
        }
    
        /// Maximal number of Gauss Points
        static const int MAX_GP_POINTS = 8;

    private: 
        /// The number of points
        int npts;
        /// The list of point payloads
        GaussPointPayload<Tp> P[MAX_GP_POINTS+1];

        /// Initialize the member P of the class.
        /// The arguments are the same as the ones for the
        /// constructor
        void Init_P(int o, int N, const Tp c[][D]);
    };

}

#endif

