
// vi: tabstop=4:expandtab

#ifndef MACI_FE_UG_SRC_DIRICHLET_ENFORCER_HH_INCLUDED
#define MACI_FE_UG_SRC_DIRICHLET_ENFORCER_HH_INCLUDED 1

#include <vector>

#include "Function.hh"
#include "VectorFunction.hh"

#include "ugNodalVector.hh"
#include "ugNodePointer.hh"
#include "ugForceCalculator.hh"


namespace UG_IF
{
    
    /// Forwarding
    class SolverImplementation;


    /// DirichletEnforcer: Enforces Dirichlet values for a set of nodes
    class DirichletEnforcer
    {

    public:
        /// Create a new force calculator
        /// @param[in]  solver  The SolverImplementation instance
        /// @param[in]  bnode   The function which defines whether or not
        ///         Dirichlet values are to be applied for a point in space
        /// @param[in]  uD  The (time dependent) Dirichlet values
        DirichletEnforcer(SolverImplementation* solver, Function<int,3>* bnode, VectorFunction<double,4,3>* uD);

        /// Destructor
        ~DirichletEnforcer();

        /// Enforce the Dirichlet values
        /// @param[in]  currtime    the current time
        void EnforceValues(double currtime);

    private:
        /// Contains the mesh
        SolverImplementation* solver;
        /// The list of nodes
        std::vector<fe::ug::NodePointer<DIM>/**/> nodes;
        /// The displacement functions
        VectorFunction<double,4,3>* uD;
    };

}

#endif

