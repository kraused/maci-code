
// vi: tabstop=4:expandtab

#ifndef POLYLIB_BOX_REFINE_HH_INCLUDED
#define POLYLIB_BOX_REFINE_HH_INCLUDED 1

namespace polylib {

    /// Refine a two dimensional box
    /// @param b    the input box
    /// @param subs the sub boxes on exit
    template<typename Tp, typename Float>
    void refineBox2(const Tp &b, Tp *subs)
    {
        Float lower[3], upper[3];
  
        lower[0] = b.getLower()[0];
        lower[1] = b.getLower()[1];
        lower[2] = 0.;
        upper[0] = b.getMidpoint(0);
        upper[1] = b.getMidpoint(1);
        upper[2] = 0.;

        subs[0].reset(lower, upper);

        lower[0] = b.getMidpoint(0);
        lower[1] = b.getLower()[1];
        lower[2] = 0.;
        upper[0] = b.getUpper()[0];
        upper[1] = b.getMidpoint(1);
        upper[2] = 0.;

        subs[1].reset(lower, upper);

        lower[0] = b.getMidpoint(0);
        lower[1] = b.getMidpoint(1);
        lower[2] = 0.;
        upper[0] = b.getUpper()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = 0.;

        subs[2].reset(lower, upper);

        lower[0] = b.getLower()[0];
        lower[1] = b.getMidpoint(1);
        lower[2] = 0.;
        upper[0] = b.getMidpoint(0);
        upper[1] = b.getUpper()[1];
        upper[2] = 0.;

        subs[3].reset(lower, upper);
    }

    /// Refine a three dimensional box
    /// @param b    the input box
    /// @param subs the sub boxes on exit
    template<typename Tp, typename Float>
    void refineBox3(const Tp &b, Tp *subs)
    {
        Float lower[3], upper[3];

        lower[0] = b.getLower()[0];
        lower[1] = b.getLower()[1];
        lower[2] = b.getLower()[2];
        upper[0] = b.getMidpoint(0);
        upper[1] = b.getMidpoint(1);
        upper[2] = b.getMidpoint(2);
    
        subs[0].reset(lower,upper);
    
        lower[0] = b.getMidpoint(0);
        lower[1] = b.getLower()[1];
        lower[2] = b.getLower()[2];
        upper[0] = b.getUpper()[0];
        upper[1] = b.getMidpoint(1);
        upper[2] = b.getMidpoint(2);
  
        subs[1].reset(lower,upper);
  
        lower[0] = b.getMidpoint(0);
        lower[1] = b.getMidpoint(1);
        lower[2] = b.getLower()[2];
        upper[0] = b.getUpper()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = b.getMidpoint(2);
 
        subs[2].reset(lower,upper);
  
        lower[0] = b.getLower()[0];
        lower[1] = b.getMidpoint(1);
        lower[2] = b.getLower()[2];
        upper[0] = b.getMidpoint(0);
        upper[1] = b.getUpper()[1];
        upper[2] = b.getMidpoint(2);
  
        subs[3].reset(lower,upper);  
  
        lower[0] = b.getLower()[0];
        lower[1] = b.getLower()[1];
        lower[2] = b.getMidpoint(2);
        upper[0] = b.getMidpoint(0);
        upper[1] = b.getMidpoint(1);
        upper[2] = b.getUpper()[2];
 
        subs[4].reset(lower,upper);
  
        lower[0] = b.getMidpoint(0);
        lower[1] = b.getLower()[1];
        lower[2] = b.getMidpoint(2);
        upper[0] = b.getUpper()[0];
        upper[1] = b.getMidpoint(1);
        upper[2] = b.getUpper()[2];
  
        subs[5].reset(lower,upper);
  
        lower[0] = b.getMidpoint(0);
        lower[1] = b.getMidpoint(1);
        lower[2] = b.getMidpoint(2);
        upper[0] = b.getUpper()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = b.getUpper()[2];
 
        subs[6].reset(lower,upper);
      
        lower[0] = b.getLower()[0];
        lower[1] = b.getMidpoint(1);
        lower[2] = b.getMidpoint(2);
        upper[0] = b.getMidpoint(0);
        upper[1] = b.getUpper()[1];
        upper[2] = b.getUpper()[2];

        subs[7].reset(lower,upper);
    }   

}

#endif

