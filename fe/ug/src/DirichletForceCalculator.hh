
// vi: tabstop=4:expandtab

#ifndef MACI_FE_UG_SRC_DIRICHLET_FORCE_HH_INCLUDED
#define MACI_FE_UG_SRC_DIRICHLET_FORCE_HH_INCLUDED 1

#include "Function.hh"
#include "VectorFunction.hh"

#include "ugNodalVector.hh"
#include "ugNodePointer.hh"
#include "ugForceCalculator.hh"


namespace UG_IF
{

    /// Forwarding
    class SolverImplementation;

    /// DirichletForceCalculator: Allows to weakly impose Dirichlet conditions
    class DirichletForceCalculator : public fe::ug::ForceCalculator<DIM>
    {

    public:
        /// Create a new force calculator
        /// @param[in]  solver  The SolverImplementation instance
        /// @param[in]  bnode   The function which defines whether or not Dirichlet
        ///                     values are to be applied for a point in space
        /// @param[in]  uD      The (time dependent) Dirichlet values
        /// @param[in]  beta    Penalty value
        DirichletForceCalculator(SolverImplementation* solver, Function<int,3>* bnode, VectorFunction<double,4,3>* uD, double beta);

        /// The copy constructor
        DirichletForceCalculator(const DirichletForceCalculator& o);

        /// Destructor
        ~DirichletForceCalculator();

        /// Clone function which overrides the pure virtual function
        /// in the base class
        DirichletForceCalculator* clone() const
        {
            return new DirichletForceCalculator(*this);
        }

        /// The eval function
        void eval(fe::ug::NodalVector<DIM>& vec);

    private:
        /// Contains the mesh
        SolverImplementation* solver;
        /// The list of nodes
        std::vector<fe::ug::NodePointer<DIM>/**/> nodes;
        /// Function which decides whether or not Dirichlet values are to be
        /// applied for a point in space
        Function<int,3>* bnode;
        /// The displacement functions
        VectorFunction<double,4,3>* uD;
        ///  Penalty factor
        double beta;
    };

}

#endif

