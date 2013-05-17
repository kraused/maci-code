
// vi: tabstop=4:expandtab

#ifndef FE_INTERFACE_UG_STATE_VARIABLES_HH_INCLUDED
#define FE_INTERFACE_UG_STATE_VARIABLES_HH_INCLUDED 1

#include "fe/ug/Solver.hh"
#include "fe/ug/Node.hh"

namespace fe {
    namespace interface {

        /// StateVariables: Class for getting access to the state variables
        ///                 like velocities and displacements
        class StateVariables
        {

        public:
            /// Return a reference to the displacement value at a 
            /// node in a direction
            /// @param[in]  solver  Reference to the solver instance
            /// @param[in]  nd  The node
            /// @param[in]  i   The direction
            static double& Displ(Solver& solver, Node& nd, int i)
            {
                return solver.GetImplementation()->U(nd, i);
            }
    
            /// Return the displacement value at a node in a direction
            /// (const version)
            /// @param[in]  solver  Reference to the solver instance
            /// @param[in]  nd      The node
            /// @param[in]  i       The direction
            static double  Displ(const Solver& solver, const Node& nd, int i)
            {
                return solver.GetImplementation()->U(nd, i);
            }

            /// Return a reference to the velocity at a node in a direction
            /// @param[in]  solver  Reference to the solver instance
            /// @param[in]  nd      The node
            /// @param[in]  i       The direction
            static double& Vel(Solver& solver, Node& nd, int i)
            {
                return solver.GetImplementation()->V(nd, i);
            }

            /// Return the velocity component in a direction at a node
            /// @param[in]  solver  Reference to the solver instance
            /// @param[in]  nd      The node
            /// @param[in]  i       The direction
            static double  Vel(const Solver& solver, const Node& nd, int i)
            {
                return solver.GetImplementation()->V(nd, i);
            }

            ///
            static double  Mass(const Solver& solver, const Node& nd)
            {
                return solver.GetImplementation()->UnweightedM(nd);
            }

            ///
            static double  WeightedMass(const Solver& solver, const Node& nd)
            {
                return solver.GetImplementation()->WeightedM(nd);
            }

            /// Return the "integration mass" of a finite element node.
            /// By definition this is the diagonal entry of the lumped
            /// mass matrix used for the time integration. Since the mass
            /// is the same for each dimension we do not pass a direction
            /// argument
            /// @param[in]  solver  Reference to the solver instance
            /// @param[in]  nd      The node
            static double  IntegratorMass(const Solver& solver, const Node& nd)
            {
                return solver.GetImplementation()->M(nd);
            }

        };

    }
}

#endif

