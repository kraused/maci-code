
// vi: tabstop=4:expandtab

#ifndef MACI_FE_UG_NODAL_VECTOR_HH_INCLUDED
#define MACI_FE_UG_NODAL_VECTOR_HH_INCLUDED 1

#include "fe/ug/Mesh.hh"
#include "fe/ug/Node.hh"

#include "ugNodalVector.hh"

namespace fe 
{
    namespace interface 
    {

        /// Forwarding
        class NodeDistributionMap;
        class Solver;

        /// NodalVector: vector used to store data attached
        ///              to nodes
        ///
        /// At each node N data items are stored
        /// 
        /// Since data is stored internally as double, care must
        /// be taken when calling the MakeConsistent() functions
        /// with Tp types other than double. However, this is not
        /// a concern for the user but only for the interface
        /// developers.
        ///
        template<typename Tp, int N = 1>
        class NodalVector : public fe::ug::NodalVector<DIM>
        {
            typedef fe::ug::NodalVector<DIM> Base;

        public:
            /// Create a new instance
            NodalVector(Mesh* mesh, Tp val = (Tp )0)
            : Base(*mesh->base, N)
            {
                // TODO Check that the type is actually supported
                //      (must fit into a double)
                // if(not NodalVectorTraits<Tp>::type_supported)
                //  error;

                    Base::operator=(val);
            }

            /// Access operator. Since UG only supports vectors
            /// storing double values we need to do this strange
            /// form of conversion
            /// @param[in]  nd  the node vector
            /// @param[in]  i   component. Set to zero by default
            ///         which is useful if scalar vectors
            ///         are used.
            Tp  operator()(const Node& nd, int i = 0) const
            {
                return *(Tp* )&Base::operator[](nd)[i];
            }

            /// Access operator (non-const version)
            /// @param[in]  nd      the node vector
            /// @param[in]  i       component. Set to zero by default
            ///                     which is useful if scalar vectors
            ///                     are used.           
            Tp& operator()(const Node& nd, int i = 0)
            {
                return *(Tp* )&Base::operator[](nd)[i];
            }

            /// Sum-up the nodal values at shared nodes
            /// @param[in]  solver  Reference to the solver class
            /// @param[in]  map Node-to-owner mapping
            void ParallelSumup(Solver& solver, const NodeDistributionMap* map);

            /// Copy the values from the primary nodes (the node instance
            /// owned by the processing element) to the copies
            /// @param[in]  solver  Reference to the solver class
            /// @param[in]  map     Node-to-owner mapping
            void ParallelCopy (Solver& solver, const NodeDistributionMap* map);

            /// Compute the maximal values at shared nodes
            /// @param[in]  solver  Reference to the solver class
            /// @param[in]  map     Node-to-owner mapping
            void ParallelMax  (Solver& solver, const NodeDistributionMap* map);

        };

    }
}

#endif

