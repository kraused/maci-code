
// vi: tabstop=4:expandtab

#include "polylib/polylib.hh"
#include "treelib/treelib.hh"
#include "halton.hh"

typedef treelib::Types<2>::Box  Box2;
typedef treelib::Types<3>::Box  Box3;

typedef treelib::Types<2>::double3  double3;

// The small brother of a real patch
template<int D>
class PatchHoldingClass : public treelib::OrthRangeTreeAttachableData<D,PatchHoldingClass<D>/**/>
{

public:
    typedef typename treelib::Types<D>::Box     Box;

    PatchHoldingClass()
    : patch_()
    {
    }
    
    PatchHoldingClass(double3 lower, double3 upper)
    : patch_(lower, upper)
    {
    }

    // Set the patch
    void setPatch(double3 lower, double3 upper)
    {
        patch_.reset(lower, upper);
    }
    
    // Get the lower corner
    const double* getLower() const
    {
        return patch_.getLower();
    }

    // Get the upper corner
    const double* getUpper() const
    {
        return patch_.getUpper();
    }

private:
    // The patch
    Box     patch_;
};

// Hierarchical construction recipe
template<int D>
class PatchConstruction : public treelib::HierarchConstr<D,PatchHoldingClass<D>/**/>
{

public:
    // Some short names
    typedef treelib::HierarchConstr<D,PatchHoldingClass<D>/**/> Base;
    typedef PatchHoldingClass<D>                                Data;

    PatchConstruction(double hdiam)
    : hdiam_(hdiam)
    {
    }

    // Construct the surrounding box
    inline void surroundingBox(const double* pt, double* lower, double* upper)
    {
        for(int d = 0; d < D; ++d) {
            lower[d] = pt[d] - hdiam_/2;
            upper[d] = pt[d] + hdiam_/2;
        }       
    }
    
    // Construct a box which is larger than the other patches
    inline void constructCoveringBox(const Data* sonobj[(1 << D)], 
                                     double* lower, double* upper)
    {
        int i;
    
        // Set the first value for comparison
        for(i = 0; i < (1 << D); ++i) {
            if(sonobj[i]) {
                for(int d = 0; d < D; ++d) {
                    lower[d] = sonobj[i]->getLower()[d];
                    upper[d] = sonobj[i]->getUpper()[d];
                }
                
                break;
            }
        }
        
        // Create the largest box
        for(++i; i < (1 << D); ++i) {
            if(sonobj[i]) {
                for(int d = 0; d < D; ++d) {
                    lower[d] = std::max(lower[d], sonobj[i]->getLower()[d]);
                    upper[d] = std::max(upper[d], sonobj[i]->getUpper()[d]);
                }
                
                break;
            }           
        }
    }

    Data* construct(const double* pt, const typename Base::Box& B, 
                    Data* obj, const Data* sonobj[(1 << D)])
    {
        double3 lower, upper;
    
        if(obj) {
            surroundingBox(pt, lower, upper);
                    
            obj->setPatch(lower, upper);
            return obj;
        } else {
            constructCoveringBox(sonobj, lower, upper);
        
            // Hugh memory leak, I know ...
            return new Data(lower, upper);
        }
    }

private:
    // Patch size for tree leafs
    double hdiam_;
};

// Require @expr to be true
#define require(expr) require_(expr, #expr, __FILE__, __LINE__);
void require_(bool expr, const char* str, const char* file, int line)
{
    if(!expr) {
        printf("%s failed at %s:%d\n", str, file, line);
        abort();
    }
}

// Query which tests if the hierarchical constructor did a good job
template<int D>
class CheckPatchesQuery : public treelib::RangeTreeQuery<D,PatchHoldingClass<D>/**/>
{

public:
    // For convenience ...
    typedef typename treelib::RangeTreeQuery<D,PatchHoldingClass<D>/**/>    Base;
    
    bool matches(const double* pt, const typename Base::Box& B, const PatchHoldingClass<D>& obj)
    {
        for(int d = 0; d < D; ++d) {
            require(obj.getUpper()[d] - obj.getLower()[d] >= 0.2 - 1e-7);
        }
        
        return true;
    }
    
    void process(const double* pt, const typename Base::Box& B, const PatchHoldingClass<D>& obj)
    {
        for(int d = 0; d < D; ++d) {
            require(fabs(obj.getLower()[d] - pt[d] + 0.1) < 1e-7);
            require(fabs(obj.getUpper()[d] - pt[d] - 0.1) < 1e-7);
        }
    }   
};

int main(int argc, char** argv)
{
    int      N   = 10000;
    double3  box = {1.,1.,1.};
    
    double3* h2  = new double3[N];
    halton(2, N, box, h2);
    double3* h3  = new double3[N];
    halton(3, N, box, h3);

    PatchHoldingClass<2>* patches2 = new PatchHoldingClass<2>[N];
    PatchHoldingClass<3>* patches3 = new PatchHoldingClass<3>[N];

    Box2 box2;
    Box3 box3;
    
    treelib::OrthRangeTree<2,PatchHoldingClass<2>/**/>  T2(box2);
    for(int i = 0; i < N; ++i) {
        T2.insert(h2[i], patches2[i]);
    }
    
    PatchConstruction<2> recipe2(0.2);
    T2.constructHierarch(recipe2);
    
    CheckPatchesQuery<2> query2;
    T2.queryTree(query2);
    
    treelib::OrthRangeTree<3,PatchHoldingClass<3>/**/>  T3(box3);
    for(int i = 0; i < N; ++i) {
        T3.insert(h3[i], patches3[i]);
    }
    
    PatchConstruction<3> recipe3(0.2);
    T3.constructHierarch(recipe3);
    
    CheckPatchesQuery<3> query3;
    T3.queryTree(query3);
}

