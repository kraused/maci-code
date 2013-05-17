
// vi: tabstop=4:expandtab


#include "DirichletEnforcer.hh"
#include "SolverImplementation.hh"
#include "Logging.hh"

using namespace UG_IF;


DirichletEnforcer::DirichletEnforcer(SolverImplementation* solver, Function<int,3>* bnode, VectorFunction<double,4,3>* uD)
: solver(solver), uD(uD->Clone())
{
    /* Create the list of nodes where bnodes is true */
    fe::ug::Mesh<DIM>* mesh = solver->GetMesh();
    for(fe::ug::NodePointer<DIM> nd = mesh->NodesBegin(); nd != mesh->NodesEnd(); ++nd)
    {
        if(1 == bnode->val(nd.getVertex()))
            nodes.push_back(nd);
    }

    Log::Instance().write("Enforcing Dirichlet values at %lu nodes.\n", nodes.size());
}

DirichletEnforcer::~DirichletEnforcer()
{
    delete uD;
}

void DirichletEnforcer::EnforceValues(double currtime)
{
    double tau;
    static double lasttime = 0.0;

    tau = currtime - lasttime;
    /* In the first step we do not have a well-defined velocity anyway, so we
       can simply set tau to 1.0. This does not affect the calculations at all */
    if(0.0 == tau)
        tau = 1.0;

    for(size_t i = 0; i < nodes.size(); ++i)
    {
        double x[4], v[3];

        x[0] = nodes[i].getVertex()[0];
        x[1] = nodes[i].getVertex()[1];
        x[2] = nodes[i].getVertex()[2];
        x[3] = currtime;

        uD->val(x, v);

        /* Approximate the the velocity by the difference quotient. This is
           basically only cosmetic for the output and does not affect the 
           correctness or accuracy of the time integration
         */
        solver->V(nodes[i],0) = (v[0] - solver->U(nodes[i],0))/tau;
        solver->V(nodes[i],1) = (v[1] - solver->U(nodes[i],1))/tau;
        solver->V(nodes[i],2) = (v[2] - solver->U(nodes[i],2))/tau;

        solver->U(nodes[i],0) = v[0];
        solver->U(nodes[i],1) = v[1];
        solver->U(nodes[i],2) = v[2];
    }

    lasttime = currtime;
}

