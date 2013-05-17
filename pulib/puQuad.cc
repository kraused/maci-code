
// vi: tabstop=4:expandtab

#include "puQuad.hh"

namespace pulib {
    namespace quadcell {

template<>
void SpecialTree<2>::insertDecomposition(BoxList<2>& list, const Box& B, const Box& b)
{
        double lower[2], upper[2];
                
        /* +---+---+---+
         * | 6 | 5 | 4 |
         * +---+---+---+
         * | 7 | b | 3 |
         * +---+---+---+
         * | 0 | 1 | 2 |
         * +---+---+---+
         */
        
        list.insertBox(b);
                
        /* i = 0 */
        lower[0] = B.getLower()[0];
        lower[1] = B.getLower()[1];
        
        upper[0] = b.getLower()[0];
        upper[1] = b.getLower()[1];
        
        list.insertBox(Box(lower,upper));
                
        /* i = 1 */
        lower[0] = b.getLower()[0];
        lower[1] = B.getLower()[1];
        
        upper[0] = b.getUpper()[0];
        upper[1] = b.getLower()[1];
        
        list.insertBox(Box(lower,upper));
        
        /* i = 2 */
        lower[0] = b.getUpper()[0];
        lower[1] = B.getLower()[1];
        
        upper[0] = B.getUpper()[0];
        upper[1] = b.getLower()[1];
        
        list.insertBox(Box(lower,upper));
        
        /* i = 3 */
        lower[0] = b.getUpper()[0];
        lower[1] = b.getLower()[1];
        
        upper[0] = B.getUpper()[0];
        upper[1] = b.getUpper()[1];
        
        list.insertBox(Box(lower,upper));
        
        /* i = 4 */
        lower[0] = b.getUpper()[0];
        lower[1] = b.getUpper()[1];
        
        upper[0] = B.getUpper()[0];
        upper[1] = B.getUpper()[1];
        
        list.insertBox(Box(lower,upper));
        
        /* i = 5 */
        lower[0] = b.getLower()[0];
        lower[1] = b.getUpper()[1];
        
        upper[0] = b.getUpper()[0];
        upper[1] = B.getUpper()[1];
        
        list.insertBox(Box(lower,upper));
        
        /* i = 6 */
        lower[0] = B.getLower()[0];
        lower[1] = b.getUpper()[1];
        
        upper[0] = b.getLower()[0];
        upper[1] = B.getUpper()[1];
        
        list.insertBox(Box(lower,upper));
        
        /* i = 7 */
        lower[0] = B.getLower()[0];
        lower[1] = b.getLower()[1];
        
        upper[0] = b.getLower()[0];
        upper[1] = b.getUpper()[1];
        
        list.insertBox(Box(lower,upper));
}


template<>
void SpecialTree<3>::insertDecomposition(BoxList<3>& list, const Box& B, const Box& b)
{
        double lower[3], upper[3], lowerz, upperz;
        
        // Attention: We actually only need the upper and lower getCorner!
        // All references to the second getCorner can be replaced by the sixth getCorner
        
        list.insertBox(b);
        
        /* bottom layer */
        lowerz = B.getLower()[2];
        upperz = b.getLower()[2];
        
        /* i = 0 */
        lower[0] = B.getLower()[0];
        lower[1] = B.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getLower()[0];
        upper[1] = b.getLower()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 1 */
        lower[0] = b.getLower()[0];
        lower[1] = B.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getUpper()[0];
        upper[1] = b.getLower()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 2 */
        lower[0] = b.getUpper()[0];
        lower[1] = B.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = B.getUpper()[0];
        upper[1] = b.getLower()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 3 */
        lower[0] = b.getUpper()[0];
        lower[1] = b.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = B.getUpper()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 4 */
        lower[0] = b.getUpper()[0];
        lower[1] = b.getUpper()[1];
        lower[2] = lowerz;
        
        upper[0] = B.getUpper()[0];
        upper[1] = B.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 5 */
        lower[0] = b.getLower()[0];
        lower[1] = b.getUpper()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getUpper()[0];
        upper[1] = B.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 6 */
        lower[0] = B.getLower()[0];
        lower[1] = b.getUpper()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getLower()[0];
        upper[1] = B.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 7 */
        lower[0] = B.getLower()[0];
        lower[1] = b.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getLower()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* middle layer */
        lowerz = b.getLower()[2];
        upperz = b.getUpper()[2];
        
        /* i = 0 */
        lower[0] = B.getLower()[0];
        lower[1] = B.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getLower()[0];
        upper[1] = b.getLower()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 1 */
        lower[0] = b.getLower()[0];
        lower[1] = B.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getUpper()[0];
        upper[1] = b.getLower()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 2 */
        lower[0] = b.getUpper()[0];
        lower[1] = B.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = B.getUpper()[0];
        upper[1] = b.getLower()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 3 */
        lower[0] = b.getUpper()[0];
        lower[1] = b.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = B.getUpper()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 4 */
        lower[0] = b.getUpper()[0];
        lower[1] = b.getUpper()[1];
        lower[2] = lowerz;
        
        upper[0] = B.getUpper()[0];
        upper[1] = B.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 5 */
        lower[0] = b.getLower()[0];
        lower[1] = b.getUpper()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getUpper()[0];
        upper[1] = B.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 6 */
        lower[0] = B.getLower()[0];
        lower[1] = b.getUpper()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getLower()[0];
        upper[1] = B.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 7 */
        lower[0] = B.getLower()[0];
        lower[1] = b.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getLower()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* upper layer */
        lowerz = b.getUpper()[2];
        upperz = B.getUpper()[2];
        
        /* i = 0 */
        lower[0] = B.getLower()[0];
        lower[1] = B.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getLower()[0];
        upper[1] = b.getLower()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 1 */
        lower[0] = b.getLower()[0];
        lower[1] = B.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getUpper()[0];
        upper[1] = b.getLower()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 2 */
        lower[0] = b.getUpper()[0];
        lower[1] = B.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = B.getUpper()[0];
        upper[1] = b.getLower()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 3 */
        lower[0] = b.getUpper()[0];
        lower[1] = b.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = B.getUpper()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 4 */
        lower[0] = b.getUpper()[0];
        lower[1] = b.getUpper()[1];
        lower[2] = lowerz;
        
        upper[0] = B.getUpper()[0];
        upper[1] = B.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 5 */
        lower[0] = b.getLower()[0];
        lower[1] = b.getUpper()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getUpper()[0];
        upper[1] = B.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 6 */
        lower[0] = B.getLower()[0];
        lower[1] = b.getUpper()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getLower()[0];
        upper[1] = B.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));
        
        /* i = 7 */
        lower[0] = B.getLower()[0];
        lower[1] = b.getLower()[1];
        lower[2] = lowerz;
        
        upper[0] = b.getLower()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = upperz;
        
        list.insertBox(Box(lower,upper));    
        
        /* Additional cuboids */
        
        lower[0] = b.getLower()[0];
        lower[1] = b.getLower()[1];
        lower[2] = B.getLower()[2];
        
        upper[0] = b.getUpper()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = b.getLower()[2];
        
        list.insertBox(Box(lower,upper));
        
        lower[0] = b.getLower()[0];
        lower[1] = b.getLower()[1];
        lower[2] = b.getUpper()[2];
        
        upper[0] = b.getUpper()[0];
        upper[1] = b.getUpper()[1];
        upper[2] = B.getUpper()[2];
        
        list.insertBox(Box(lower,upper));   
}

}

}


