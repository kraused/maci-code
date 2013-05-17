
// vi: tabstop=4:expandtab

#ifndef PULIB_BASIS_FUNCTION_HH_INCLUDED
#define PULIB_BASIS_FUNCTION_HH_INCLUDED 1

#include "pulib/typedefs.hh"
#include "hash/hash.hh"
#include "puPatch.hh"

namespace pulib
{

// The generating function of the PU space
template<int D>
class BasisFunction : public Patch<D>
{

public:
    typedef typename Types<D>::double3  double3;

    // Construct a new function supported on a patch
    // surrounding @pt with nonisotropic sidelength @hJ
    BasisFunction(const double* pt, const double* hJ, int id);

    // Free memory
    ~BasisFunction();

    // Get the point; Note that this function is
    // implemented twice but this is necessary because
    // the @getPoint() function can only be called after
    // the point has been inserted in the tree
    inline const double3& getParticlePt() const
    {
        return pt_;
    }
    
    // Set the number of overlapping patches
    // This function also allocates the storage; Therefore
    // it should be called only once!
    inline void setNumOvPatches(int num);
        
    // Get the number of overlapping patches
    // The own patch is of course always in this list (if the insert
    // was correct)!
    inline int getNumOvPatches() const
    {
        return numOvPatches_;
    }
    
    // Insert a neighbour patch
    inline void setOvPatch(int i, const Patch<D>* p)
    {
        ovPatches_[i] = p;
    }
    
    // Returns a pointer to the overlapping patch with
    // local id @i
    inline const Patch<D>* getOvPatch(int i) const
    {
        return ovPatches_[i];
    }
    
public:
    // Iterator for the overlapping patches
    class OvIterator
    {
    public:
        OvIterator(const BasisFunction<D>* phi, int start)
        : phi_(phi), pos_(start)
        {
        }
        
        OvIterator(const OvIterator& it)
        : phi_(it.phi_), pos_(it.pos_)
        {
        }
        
        // Compare two iterators. This translates
        // to a comparison of the positions
        bool operator<(const OvIterator& it) const
        {
            return pos_ < (it.pos_);
        }
        
        // ++it operator
        OvIterator& operator++()
        {
            ++pos_;
            return *this;
        }
        
        // it++ operator
        OvIterator operator++(int)
        {
            OvIterator tmp = *this;
            ++pos_;
            return tmp;
        }
        
        // In the same manner one might add --
        
        const Patch<D>& operator*() const
        {
            return *phi_->getOvPatch(pos_);
        }
        
        const Patch<D>* operator->() const
        {
            return phi_->getOvPatch(pos_);
        }
        
        // Comparison operator
        bool operator!=(const OvIterator& it) const
        {
            return pos_ != it.pos_;
        }
        
        bool operator==(const OvIterator& it) const
        {
            return pos_ == it.pos_;
        }
            
    private:
        // The generating function itself
        const BasisFunction<D>* phi_;
        // Position of the iterator
        int         pos_;
    };
    
    // Returns the first overlapping patch
    inline OvIterator begin() const
    {
        return OvIterator(this, 0);
    }
    
    // Returns the end of neighbour list
    inline OvIterator end() const
    {
        return OvIterator(this, numOvPatches_);
    }       
    
private:
    // The list of overlapping patches
    const Patch<D>**    ovPatches_;
    // The number of neighbors; This is stored
    // seperatly to ensure O(1) access
    int         numOvPatches_;
    // The particle position
    double3         pt_;

};

// The generating system of a PU space
template<int D>
class Basis
{
    typedef hash::Hashmap<int,BasisFunction<D>/**/> Hashmap;

public:
    typedef typename Types<D>::double3      double3;
    
    // Create a new system. By default, the basis is empty and must be
    // filled in using @insert.
    //
    // Note: The generating system is *not* complete
    //       when the constructor is finished. In fact
    //       you need to query the list of overlapping
    //       patches by hand.
    Basis(double3* pt, double3* hJ, int* idx, int N);
    
    // Destroy the system
    ~Basis();
    
    // Get the cardinality of the system
    inline int size() const
    {
        return size_;
    }
    
    // Get the maximal sidelength of a patch
    // for each dimension
    void getMaxPatchSidelength(double3 hJ) const;

    // An iterator over the functions
    typedef BasisFunction<D>*   Iterator;
    typedef const BasisFunction<D>* ConstIterator;

    // Returns the first generating function
    inline Iterator begin()
    {
        return phiJ_;
    }
    inline ConstIterator begin() const
    {
        return phiJ_;
    }
    
    // Returns an iterator pointing beyound the end
    // of the array of generating functions
    inline Iterator end()
    {
        return (phiJ_ + size_);
    }
    inline ConstIterator end() const
    {
        return (phiJ_ + size_);
    }
    
    // Print some statistics
    void printStatistic() const;

    // Access to iterator by index
    // Important note: Indices are always total and *not* relative the start
    //         of the index range
    // If @id is invalid (e.g. out of range), then an exception with number
    // Hashmap::INVALID_KEY is thrown
    inline Iterator getFunction(int id)
    {
        return hashmap_(id);
    }
    inline ConstIterator getFunction(int id) const
    {
        return hashmap_(id);        
    }

private:
    // List of generating function
    BasisFunction<D>*   phiJ_;
    // The number of functions
    int         size_;
    // A hash map which maps the index of a PU to its position in @phiJ
    Hashmap         hashmap_;
    // log(size of hashmap)
    static const int    HASHMAP_LOGSIZE = 8;
};


template<int D>
BasisFunction<D>::BasisFunction(const double* pt, const double* hJ, int id)
: Patch<D>(id), ovPatches_(0), numOvPatches_(0)
{
    for(int d = 0; d < D; ++d) {
        pt_[d]   = pt[d];
    }

    double3 lower, upper;
    for(int d = 0; d < D; ++d) {
        lower[d] = pt[d] - 0.5*hJ[d];
        upper[d] = pt[d] + 0.5*hJ[d];
    }
    
    reset(lower, upper);
}

template<int D>
BasisFunction<D>::~BasisFunction()
{
    if(ovPatches_) {
        delete[] ovPatches_;
    }
}

template<int D>
inline void BasisFunction<D>::setNumOvPatches(int num)
{
    ASSERT(0 < num);
    ASSERT(0 == ovPatches_);
    
    numOvPatches_ = num;
    ovPatches_    = new const Patch<D>*[numOvPatches_];
}


template<int D>
Basis<D>::Basis(double3* pt, double3* hJ, int* idx, int N)
: phiJ_((BasisFunction<D>* )malloc(N*sizeof(BasisFunction<D>))), size_(N), 
  hashmap_(HASHMAP_LOGSIZE)
{
    for(int i = 0; i < N; ++i) {
        new(&phiJ_[i]) BasisFunction<D>(pt[i], hJ[i], idx[i]);
        hashmap_.add(idx[i], &phiJ_[i]);
    }
}

template<int D>
Basis<D>::~Basis()
{
    for(Iterator it = begin(); it != end(); ++it) {
        it->~BasisFunction<D>();
    }
    free(phiJ_); phiJ_ = 0;
}

template<int D>
void Basis<D>::getMaxPatchSidelength(double3 hJ) const
{
    ConstIterator it = begin();

    for(int d = 0; d < D; ++d) {
        hJ[d] = it->getUpper()[d] - it->getLower()[d];
    }
    
    for(; it != end(); ++it) {
        for(int d = 0; d < D; ++d) {
            hJ[d] = std::max(it->getUpper()[d] - it->getLower()[d], hJ[d]);
        }
    }
}

}

#endif

