
// vi: tabstop=4:expandtab

#ifndef PULIB_QUAD_HH_INCLUDED
#define PULIB_QUAD_HH_INCLUDED 1

#include "pulib/typedefs.hh"
#include "pulib/puPatch.hh"
#include "pulib/puBasisFunction.hh"

#include <list>

namespace pulib
{

/// @warning    The regularity of the shape functions depends critically
///     on the weight function. It is the tensor property which
///     forces us to refine the boxes in the algorithm. The function
///     <code>
///     computeQuadCell
///     </code>
///     might not yield correct results for some weighting functions!

// Compute quadrature cells for the function @phi
// @ncells is the number of cells (return value)
// @cells is the array of cells. It is allocated with new[] and
// should be released by the user
template<int D>
void computeQuadCells(const BasisFunction<D>& phi, int* ncells, typename Types<D>::Box** cells);


namespace quadcell
{

/** A small number */
static const double EPSILON = 1e-7;

// A list of boxes for internal use
template<int D>
class BoxList : public std::list<typename pulib::Types<D>::Box>
{
    typedef typename Types<D>::Box  Box;
    typedef std::list<Box>      Base;

public:
    typedef typename Base::iterator     Iterator;
    typedef typename Base::const_iterator   ConstIterator;

public:
    BoxList()
    : Base()
    {
    }
    
    BoxList(const BoxList& list)
    : Base(list)
    {
    }
    
    // Insert a new box
    void insertBox(const Box& b)
    {
        if(b.getVolume() >= VOL_THRESHOLD)
            push_back(b);
    }
    
private:
    // Boxes with volume below this threshold are
    // rejected
    static const double VOL_THRESHOLD;
};

template<int D>
const double BoxList<D>::VOL_THRESHOLD = 1e-10;


// A special tree with two levels
// used to compute the intersection
// with remote 
template<int D>
class SpecialTree
{
    typedef typename Types<D>::Box          Box;
    typedef typename BoxList<D>::Iterator       BoxIterator;
    typedef typename BoxList<D>::ConstIterator  ConstBoxIterator;

public:
    // Construct a new tree
    SpecialTree(const Box& b)
    {
        b.refine(boxes_);
            
        for(int i = 0; i < (1<<D); ++i) {
            cells_[i].insertBox(boxes_[i]);
            vols_[i] = boxes_[i].getVolume();
        } 
        
#ifndef NDEBUG
        double vol = 0.;
        for(int i = 0; i < (1<<D); ++i) {
            vol += boxes_[i].getVolume();
        }

        ASSERT(fabs(vol - b.getVolume()) < 1e-5);
#endif
    }
    
    // Replace elements with intersection
    void replaceIntersectingBoxes(const Box& b)
    {
        double vol = b.getVolume();
    
        for(int i = 0; i < (1<<D); ++i) {
            if(boxes_[i].hasIntersection(b,EPSILON*std::min(vols_[i],vol))) {
                replaceIntersectingBoxesOnLevel2(i, b, vol);
            }
        }
    }
    
    // Get number of integration cells
    inline int getNumCells() const
    {
        int num = 0;
        for(int i = 0; i < (1<<D); ++i) {
            num += cells_[i].size();
        }
        
        return num;
    }
    
    // Get all cells as an array
    Box* getCellsAsArray() const
    {
        Box* array = new Box[getNumCells()]();
        int ii = 0;
        
        for(int i = 0; i < (1<<D); ++i) {
            for(ConstBoxIterator it = cells_[i].begin(); it != cells_[i].end(); ++it) {
                array[ii++].reset(it->getLower(),it->getUpper());
            }
        }
        
        return array;
    }
    
private:
    // Support function for @replaceIntersectingBoxes
    // This function does the actual work
    void replaceIntersectingBoxesOnLevel2(int i, const Box& b, double vol)
    {   
        int n = 0;
        
        if(/*unlikely*/(MAX_CELLS <= (int )cells_[i].size())) {
            printf(" Error: cells_[i].size() >= MAX_CELLS in %s:%d\n", __FILE__, __LINE__);
            abort();
        }
        
        for(BoxIterator it = cells_[i].begin(); it != cells_[i].end(); ++it) {
            if(it->hasIntersection(b, EPSILON*std::min(it->getVolume(), vol))) {
                marked_[n++] = it;
            }
        }
        
        for(int j = 0; j < n; ++j) {
            Box B(*marked_[j]);
            Box cut = B.getIntersection(b, EPSILON*std::min(B.getVolume(), vol));
            
            ASSERT(cut.getVolume() >= EPSILON*std::min(B.getVolume(), vol));
            
            // Need to erase this one first because the iterator becomes
            // invalid after the insert of new elements
            cells_[i].erase(marked_[j]);
            
            insertDecomposition(cells_[i], B, cut);
        }
    }
    
    // Insert the decomposition of @B given by @b and its complement into the list @list
    void insertDecomposition(BoxList<D>& list, const Box& B, const Box& b);
        
private:
    // The maximal number of cells that we can deal with
    static const int MAX_CELLS = 128;
    // A small arena for the flags (marking of cells)
    BoxIterator marked_[MAX_CELLS];
    // The boxes on Level 1
    Box     boxes_[(1<<D)];
    // The volumes of the boxes
    double      vols_ [(1<<D)];
    // List of cells with numbering
    // consistent with the one of @boxes
    BoxList<D>  cells_[(1<<D)];
};

}

template<int D>
void computeQuadCells(const BasisFunction<D>& phi, int* ncells, typename Types<D>::Box** cells)
{
    quadcell::SpecialTree<D> T(phi);
    
    for(typename BasisFunction<D>::OvIterator it = phi.begin(); it != phi.end(); ++it) {
        // Skip yourself
        if(it->getId() == phi.getId()) {
            continue;
        }
    
        // Get refinement
        typename Types<D>::Box boxes[(1<<D)];
        it->refine(boxes);
        
        for(int i = 0; i < (1<<D); ++i) {
            T.replaceIntersectingBoxes(boxes[i]);
        }
    }
    
    *ncells = T.getNumCells();
    *cells  = T.getCellsAsArray();
}

}

#endif

