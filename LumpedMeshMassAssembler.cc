
// vi: tabstop=4:expandtab

#include <numeric>

#include "LumpedMeshMassAssembler.hh"


void LumpedMeshMassAssembler::AssembleMatrix(ElementList& list, Function<double, 3>& f)
{
    const int MAX_NC = fe::interface::MAX_CORNERS_OF_ELEMENT;
    int k;
    double localMat[MAX_NC*MAX_NC];
    int64 globali;

    /// Important: All vector elements must be initialized with zero!
    fe::interface::NodalVector<double, 1> vector(gcpe()->GetFESolver()->GetMesh(), 0.0);

    for(ElementList::ConstIterator i = list.begin(); i != list.end(); ++i)
    {
        AssembleOnElement(*i, f, localMat);
        for(k = 0; k < i->ncorners(); ++k)
            vector(i->corner(k)) += std::accumulate(&localMat[k*i->ncorners()], &localMat[(k+1)*i->ncorners()], 0.0);
    }

    vector.ParallelSumup(*gcpe()->GetFESolver(), gcpe()->GetFESolver()->GetNodeDistribMap());

    for(ElementList::ConstIterator i = list.begin(); i != list.end(); ++i)
        for(k = 0; k < i->ncorners(); ++k)
            if(gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(i->corner(k)) and 0.0 != vector(i->corner(k)))
            {
                globali = Int64::concat(gcpe()->world.myrank(), Node::Index(*gcpe()->GetFESolver(), i->corner(k)));
                
                matrix.insert(globali, globali, vector(i->corner(k)));
                /// Set the vector value to zero to make sure that values are 
                /// not inserted twice
                vector(i->corner(k)) = 0.0;
            }
}

void LumpedMeshMassAssembler::AssembleMatrixUsingFEMass(NodeList& list)
{
    int64 globali;

    for(NodeList::ConstIterator i = list.begin(); i != list.end(); ++i)
        if(gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(*i))
        {
            globali = Int64::concat(gcpe()->world.myrank(), Node::Index(*gcpe()->GetFESolver(), *i));
            matrix.insert(globali, globali, fe::interface::StateVariables::Mass(*gcpe()->GetFESolver(), *i));
        }
}

void LumpedMeshMassAssembler::AssembleMatrixUsingFEWeightedMass(NodeList& list)
{
    int64 globali;

    for(NodeList::ConstIterator i = list.begin(); i != list.end(); ++i)
        if(gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(*i))
        {
            globali = Int64::concat(gcpe()->world.myrank(), Node::Index(*gcpe()->GetFESolver(), *i));
            matrix.insert(globali, globali, fe::interface::StateVariables::WeightedMass(*gcpe()->GetFESolver(), *i));
        }
}

