
// vi: tabstop=4:expandtab

#include "puVisualize.hh"

namespace pulib
{

template<int D>
class PlotTreeQuery : public Query<D>
{

public:
    typedef typename treelib::OrthRangeTree<D,Patch<D>/**/>::Box    Box;

    PlotTreeQuery(Visualizer<D>& vis)
    : vis_(vis)
    {
    }
    
    bool matches(const double* pt, const Box& B, const Patch<D>& obj)
    {
        vis_.plotBox(B);
        return true;
    }

    void process(const double* pt, const Box& B, const Patch<D>& obj)
    {
        vis_.plotPoint(pt);
    }
        
private:
    // The visualizer
    Visualizer<D>&  vis_;
};

template<int D>
void plotTree(ApproximationSpace<D>& X, Visualizer<D>& vis)
{
    PlotTreeQuery<D>    query(vis);
    X.queryTreeLocal(query);
}

template
void plotTree<2>(ApproximationSpace<2>& X, Visualizer<2>& vis);

template
void plotTree<3>(ApproximationSpace<3>& X, Visualizer<3>& vis);


template<int D>
void plotPatches(ApproximationSpace<D>& X, Visualizer<D>& vis)
{
    for(typename ApproximationSpace<D>::ConstIterator it = X.begin(); it != X.end(); ++it) {
        vis.plotPoint(it->getParticlePt());
        vis.plotBox(*it);
    }
}

template
void plotPatches<2>(ApproximationSpace<2>& X, Visualizer<2>& vis);

template
void plotPatches<3>(ApproximationSpace<3>& X, Visualizer<3>& vis);


template<int D>
class PlotPatchHierarchyQuery : public Query<D>
{

public:
    typedef typename treelib::OrthRangeTree<D,Patch<D>/**/>::Box    Box;

    PlotPatchHierarchyQuery(Visualizer<D>& vis)
    : vis_(vis)
    {
    }
    
    bool matches(const double* pt, const Box& B, const Patch<D>& obj)
    {
        vis_.plotBox(obj);
        return true;
    }

    void process(const double* pt, const Box& B, const Patch<D>& obj)
    {
        vis_.plotPoint(pt);
    }
        
private:
    // The visualizer
    Visualizer<D>&  vis_;
};

template<int D>
void plotPatchHierarchy(ApproximationSpace<D>& X, Visualizer<D>& vis)
{
    PlotPatchHierarchyQuery<D>  query(vis);
    X.queryTreeLocal(query);
}

template
void plotPatchHierarchy<2>(ApproximationSpace<2>& X, Visualizer<2>& vis);

template
void plotPatchHierarchy<3>(ApproximationSpace<3>& X, Visualizer<3>& vis);

}

