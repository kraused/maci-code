
// vi: tabstop=4:expandtab

#include "NeumannForceCalculator.hh"
#include "SolverImplementation.hh"

#include "ugElementPointer.hh"
#include "ugElementSide.hh"

using namespace UG_IF;


NeumannForceCalculator::NeumannForceCalculator(SolverImplementation* solver, Function<int,3>* bapply, VectorFunction<double,4,3>* F)
: solver(solver), bapply(bapply->Clone()), F(F->Clone())
{
}

NeumannForceCalculator::NeumannForceCalculator(const NeumannForceCalculator& other)
: solver(other.solver), bapply(other.bapply->Clone()), F(other.F->Clone())
{
}

NeumannForceCalculator::~NeumannForceCalculator()
{
    delete bapply;
    delete F;
}

void NeumannForceCalculator::eval(fe::ug::NodalVector<DIM>& vec)
{
    vec = 0.0;

    fe::ug::Mesh<DIM>* mesh = solver->GetMesh();

    for(fe::ug::ElementPointer<DIM> el = mesh->ElementsBegin(); el != mesh->ElementsEnd(); ++el)
        for(fe::ug::ElementSide<DIM> S = el.SidesBegin(); S != el.SidesEnd(); ++S)
            if(S.isBorderSide())
            {
                double x[4], v[3], f;

                S.getMidpoint(x);
                x[3] = solver->currtime;

                if(1 != bapply->val(x))
                    continue;

                F->val(x,v);

                f = S.area()/(double )S.GetNumCorners();

                for(int i = 0; i < S.GetNumCorners(); ++i)
                {
                    fe::ug::NodePointer<DIM> nd = S.corner(i);

                    vec[nd][0] += f*v[0];
                    vec[nd][1] += f*v[1];
                    vec[nd][2] += f*v[2];
                }
            }
    
    vec.MakeConsistent(fe::ug::NodalVector<DIM>::SUM);
}

