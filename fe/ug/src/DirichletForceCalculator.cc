
// vi: tabstop=4:expandtab

#include "DirichletForceCalculator.hh"
#include "SolverImplementation.hh"
#include "Logging.hh"

#include "ugElementPointer.hh"
#include "ugElementSide.hh"

using namespace UG_IF;


DirichletForceCalculator::DirichletForceCalculator(SolverImplementation* solver, Function<int,3>* bnode, VectorFunction<double,4,3>* uD, double beta)
: solver(solver), bnode(bnode->Clone()), uD(uD->Clone()), beta(beta)
{
    /* Create the list of nodes where bnodes is true */
    fe::ug::Mesh<DIM>* mesh = solver->GetMesh();
    for(fe::ug::NodePointer<DIM> nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
    {
        if(1 == bnode->val(nd.getVertex()))
            nodes.push_back(nd);
    }

    Log::Instance().write("Weakly enforcing Dirichlet values at %lu nodes.\n", nodes.size());
}

DirichletForceCalculator::DirichletForceCalculator(const DirichletForceCalculator& other)
: solver(other.solver), nodes(other.nodes), bnode(other.bnode->Clone()), uD(other.uD->Clone()), beta(other.beta)
{
}

DirichletForceCalculator::~DirichletForceCalculator()
{
    delete uD;
}

void DirichletForceCalculator::eval(fe::ug::NodalVector<DIM>& vec)
{
#if 0
    size_t i;
    double x[4], v[3];

    vec = 0.0;
    for(i = 0; i < nodes.size(); ++i)
    {
        x[0] = nodes[i].getVertex()[0];
        x[1] = nodes[i].getVertex()[1];
        x[2] = nodes[i].getVertex()[2];
        x[3] = solver->currtime;

        uD->val(x, v);

        /// Note that we are using the unweighted FE masses whether or not the equations
        /// of motions are weighted. Anyway, in most cases it doesn't make a difference
        /// since Dirichlet values are typically not enforced in the coupling zone.
        vec[nodes[i]][0] = beta*solver->UnweightedM(nodes[i])*(solver->U(nodes[i],0) - v[0]);
        vec[nodes[i]][1] = beta*solver->UnweightedM(nodes[i])*(solver->U(nodes[i],1) - v[1]);
        vec[nodes[i]][2] = beta*solver->UnweightedM(nodes[i])*(solver->U(nodes[i],2) - v[2]);
    }
#else
    fe::ug::Mesh<DIM>* mesh = solver->GetMesh();

    vec = 0.0;
    for(fe::ug::ElementPointer<DIM> el = mesh->ElementsBegin(); el != mesh->ElementsEnd(); ++el)
        for(fe::ug::ElementSide<DIM> S = el.SidesBegin(); S != el.SidesEnd(); ++S)
            if(S.isBorderSide())
            {
                double x[4], v[3], f;

                f = S.area()/(double )S.GetNumCorners();

                for(int i = 0; i < S.GetNumCorners(); ++i)
                {
                    fe::ug::NodePointer<DIM> nd = S.corner(i);

                    if(0 == bnode->val(nd.getVertex()))
                        continue;

                    x[0] = nd.getVertex()[0];
                    x[1] = nd.getVertex()[1];
                    x[2] = nd.getVertex()[2];
                    x[3] = solver->currtime;

                    uD->val(x, v);

                    vec[nd][0] += f*beta*(solver->U(nd,0) - v[0]);
                    vec[nd][1] += f*beta*(solver->U(nd,1) - v[1]);
                    vec[nd][2] += f*beta*(solver->U(nd,2) - v[2]);
                }
            }

    vec.MakeConsistent(fe::ug::NodalVector<DIM>::SUM);
#endif
}

