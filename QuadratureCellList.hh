
// vi: tabstop=4:expandtab

#ifndef MACI_QUADRATURE_CELL_LIST_HH_INCLUDED
#define MACI_QUADRATURE_CELL_LIST_HH_INCLUDED 1

#include "PUSpace.hh"
#include "DesignByContract.hh"
#include "Delete.hh"

#include "hash/hash.hh"


/// Instances of this class manages quadrature cell list for the assembling of
/// the L2 projection using a partition of unity space
/// Since we use lower-order splines as weight functions in Shepards approach
/// the basis functions of the Partition of unity space are not differentiable
/// To do quadrature we subdivide the support of the function into pieces on
/// which the function is smooth and use summed quadrature to evaluate the
/// integrals
class QuadratureCellList
{
    typedef pulib::Types<DIM>::Box  Box;
    typedef hash::Hashmap<int,int>      Hashmap;

public:
    /// Create a new instance
    /// @param space    the partition of unity space
    QuadratureCellList(const PUSpace& space)
    : size(space.getSizeLocalBasis()), qcells_(0), num_(0), space_(space),
      hashmap_(HASHMAP_LOGSIZE)
    {
        CHECK_PRECOND_ALWAYS(space.getSizeLocalBasis() >= 0);

        if(space.getSizeLocalBasis() > 0) {
            qcells_ = new Box*[size];
            num_    = new  int[size];
        }

        int cnt = 0;
        for(PUSpace::ConstIterator it = space.begin(); it != space.end(); ++it, ++cnt) 
        {
            pulib::computeQuadCells(*it, &num_[cnt], &qcells_[cnt]);
            hashmap_.add(it->getId(), cnt);

#ifndef NDEBUG
            {
                double vol0 = it->getVolume();
                double vol1 = 0.;
                for(int i = 0; i < num_[cnt]; ++i) {
                    vol1 += qcells_[cnt][i].getVolume();
                }

                CHECK_DEBUG(fabs(vol0 - vol1) < 1e-2*vol0, 0);
            }
#endif
        }
    }
    
    /// Delete the instance
    ~QuadratureCellList()
    {
        for(int i = 0; i < getNumSublists(); ++i) 
        {
            CHECK_PRECOND_ALWAYS(qcells_[i]);
            delete[] qcells_[i];
        }

        DeleteArray(qcells_);
        DeleteArray(num_);
    }
    
    /// Get number of sublists
    /// @returns int >= 0
    inline int getNumSublists() const
    {
        CHECK_PRECOND_DEBUG(size >= 0);
        return size;
    }
    
    typedef Box*        Iterator;
    typedef const Box*  ConstIterator;
    
    /// Get first quadrature cell for a given patch
    /// @param obj  the patch
    /// @returns    iterator pointing to start
    Iterator begin(const pulib::Patch<DIM>& obj)
    {
        CHECK_PRECOND_ALWAYS(qcells_);

        int n = *hashmap_(obj.getId());
        CHECK_DEBUG(n >= 0 && n < size, 0);
        return qcells_[n];
    }

    /// Get first quadrature cell for a given patch (const version)
    /// @param obj  the patch
    /// @returns    const iterator pointing to start
    ConstIterator begin(const pulib::Patch<DIM>& obj) const
    {
        CHECK_PRECOND_ALWAYS(qcells_);

        int n = *hashmap_(obj.getId());
        CHECK_DEBUG(n >= 0 && n < size, 0);
        return qcells_[n];
    } 
    
    /// Get end of quadrature cell list for a given patch
    /// @param obj  the patch
    /// @returns    iterator pointing to end
    Iterator end(const pulib::Patch<DIM>& obj)
    {
        CHECK_PRECOND_ALWAYS(qcells_);

        int n = *hashmap_(obj.getId());
        CHECK_DEBUG(n >= 0 && n < size, 0);
        return (qcells_[n] + num_[n]);
    }

    /// Get end of quadrature cell list for a given patch (const version)
    /// @param obj  the patch
    /// @returns    const iterator pointing to end
    ConstIterator end(const pulib::Patch<DIM>& obj) const
    {
        CHECK_PRECOND_ALWAYS(qcells_);

        int n = *hashmap_(obj.getId());
        CHECK_DEBUG(n >= 0 && n < size, 0);
        return (qcells_[n] + num_[n]);
    }

private:
    /// Number of local basis functions
    int size;
    /// The lists of quadratur cells
    Box **qcells_;
    /// The number of quad cells
    int *num_;
    /// Reference to the puspace
    const PUSpace& space_;
    /// Hash map connecting patches and the index in the list of quadrature
    /// cells
    Hashmap hashmap_;
    
    /// log(size of hashmap)
    static const int HASHMAP_LOGSIZE = 8;
};

#endif

