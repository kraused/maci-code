
// vi: tabstop=4:expandtab

#include "polylib/polylib.hh"
#include "treelib/treelib.hh"
#include "halton.hh"

#include <algorithm>

typedef treelib::Types<2>::Box  Box2;
typedef treelib::Types<3>::Box  Box3;

typedef treelib::Types<2>::double3  double3;

// Class that associates ids to points
template<int D>
class CountingClass : public treelib::OrthRangeTreeAttachableData<D,CountingClass<D>/**/>
{

public:
    CountingClass()
    : id_(0)
    {
    
    }

    CountingClass(int id)
    : id_(id)
    {
    
    }
    
    // Get the id
    inline int getId() const
    {
        return id_;
    }
    
    // Set the id
    inline void setId(int id)
    {
        id_ = id;
    }
    
private:
    // The id of the class
    int     id_;
};

// Return the maximal number of 3 doubles
inline double max3(double x, double y, double z)
{
    return std::max(std::max(x,y), std::max(y,z));
}

// linfty distance between points
double distance(const double3 x, const double3 y) 
{
    return max3(fabs(x[0] - y[0]), fabs(x[1] - y[1]), fabs(x[2] - y[2]));
} 

// linfinty range query
template<int D>
class LiRangeQuery : public treelib::RangeTreeQuery<D,CountingClass<D>/**/>
{

public:
    // For convenience ...
    typedef typename treelib::RangeTreeQuery<D,CountingClass<D>/**/>    Base;

    LiRangeQuery(const double3& pt, double r)
    : r_(r)
    {
        double3 lower, upper;
        
        memcpy(pt_, pt, 3*sizeof(double));
        
        lower[0] = pt[0] - r;
        lower[1] = pt[1] - r;
        lower[2] = pt[2] - r;
        
        upper[0] = pt[0] + r;
        upper[1] = pt[1] + r;
        upper[2] = pt[2] + r;

        box_ = typename Base::Box(lower,upper);
    }
        
    bool matches(const double* pt, const typename Base::Box& B, const CountingClass<D>& obj)
    {
        return box_.hasIntersectionUnsafe(B);
    }
    
    void process(const double* pt, const typename Base::Box& B, const CountingClass<D>& obj)
    {
        if(distance(pt,pt_) < r_) {
            neighbours_.push_back(obj.getId());
        }
    }
    
    // Begin of the neighbour list
    std::list<int>::iterator begin()
    {
        return neighbours_.begin();
    }
    
    // End of the neighbour list
    std::list<int>::iterator end()
    {
        return neighbours_.end();
    }
    
private:
    // The point to look at
    double3         pt_;
    // The radius
    double          r_;
    // Box used to detect candidates
    typename Base::Box  box_;
    // The list of neighbours
    std::list<int>      neighbours_;
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

int main(int argc, char** argv)
{
    int      N   = 1000;
    double3  box = {1.,1.,1.};
    
    double3* h2  = new double3[N]();
    halton(2, N, box, h2);
    double3* h3  = new double3[N]();
    halton(3, N, box, h3);

    Box2 box2;
    Box3 box3;

    srand(0);
    int i = rand();
    
    CountingClass<2>* cnt2 = new CountingClass<2>[N];
    CountingClass<3>* cnt3 = new CountingClass<3>[N];
    
    
    treelib::OrthRangeTree<2,CountingClass<2>/**/>  T2(box2);
    for(int i = 0; i < N; ++i) {
        cnt2[i].setId(i);
        T2.insert(h2[i], cnt2[i]);
    }
    
    LiRangeQuery<2> query2(h2[i], 0.2);
    T2.queryTree(query2);
    
    for(std::list<int>::iterator it = query2.begin(); it != query2.end(); ++it) {
        require(*it >= 0 and *it < N);
        require(distance(h2[i], h2[*it]) < 0.2);
        
        for(int k = 0; k < N; ++k) {
            if(distance(h2[i], h2[k]) < 0.2) {
                require(std::find(query2.begin(), query2.end(), k) != query2.end());
            }
        }
    }
    
    treelib::OrthRangeTree<3,CountingClass<3>/**/>  T3(box3);
    for(int i = 0; i < N; ++i) {
        cnt3[i].setId(i);
        T3.insert(h3[i], cnt3[i]);
    }
    
    LiRangeQuery<3> query3(h3[i], 0.2);
    T3.queryTree(query3);
    
    for(std::list<int>::iterator it = query3.begin(); it != query3.end(); ++it) {
        require(*it >= 0 and *it < N);
        require(distance(h2[i], h2[*it]) < 0.2);
        
        for(int k = 0; k < N; ++k) {
            if(distance(h2[i], h2[k]) < 0.2) {
                require(std::find(query2.begin(), query2.end(), k) != query2.end());
            }
        }       
    }
}

