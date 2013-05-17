
// vi: tabstop=4:expandtab

#ifndef MACI_FE_UG_SRC_NEUMANN_FORCE_HH_INCLUDED
#define MACI_FE_UG_SRC_NEUMANN_FORCE_HH_INCLUDED 1

#include "Function.hh"
#include "VectorFunction.hh"

#include "ugNodalVector.hh"
#include "ugNodePointer.hh"
#include "ugForceCalculator.hh"


namespace UG_IF
{

    /// Forwarding
    class SolverImplementation;

    /// NeumannForceCalculator: Adds Neumann (surface forces) to the
    ///             right hand side
    class NeumannForceCalculator : public fe::ug::ForceCalculator<DIM>
    {

    public:
        /// Create a new force calculator
        /// @param[in]  solver  The SolverImplementation instance
        /// @param[in]  bapply  The function which defines whether or not
        ///                     the Neummann forces apply to a point in space
        /// @param[in]  F       The Neumann force function (time dependent)
        NeumannForceCalculator(SolverImplementation* solver, Function<int,3>* bapply, VectorFunction<double,4,3>* F);

        /// The copy constructor
        NeumannForceCalculator(const NeumannForceCalculator& o);

        /// Destructor
        ~NeumannForceCalculator();

        /// Clone function which overrides the pure virtual function
        /// in the base class
        NeumannForceCalculator* clone() const
        {
            return new NeumannForceCalculator(*this);
        }

        /// The eval function
        void eval(fe::ug::NodalVector<DIM>& vec);

    private:
        /// Contains the mesh
        SolverImplementation* solver;
        /// Function which decides whether or not the Neumann forces apply to a point
        /// in space
        Function<int,3>* bapply;
        /// The force function. This is evaluated in each timestep
        VectorFunction<double,4,3>* F;
    };

}

#endif

