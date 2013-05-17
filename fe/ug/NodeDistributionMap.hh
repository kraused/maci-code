
// vi: tabstop=4:expandtab

#ifndef MAXI_FE_INTERFACE_UG_NODE_DISTRIBUTION_MAP_HH_INCLUDED
#define MAXI_FE_INTERFACE_UG_NODE_DISTRIBUTION_MAP_HH_INCLUDED 1

#include "fe/ug/NodalVector.hh"
#include "fe/ug/Node.hh"

#include <stdint.h>

namespace fe {
    namespace interface {

        /// Forwarding
        class Solver;

        /// NodeDistributionMap: Defines a mapping from nodes to
        ///              the owner process
        ///
        /// We use an int64_t so that we don't need a temporary
        /// vector in CreateMappingFromPriorities()
        ///
        class NodeDistributionMap : public NodalVector<int64_t>
        {
            typedef NodalVector<int64_t> Base;

        public:
            /// Create a new instance
            NodeDistributionMap(Mesh* M);

            /// Create a new mapping given "priorities" for
            /// each node. This means that the processing elements
            /// can assign a scalar value to each node representing
            /// how "strong the desire is to own the node". The
            /// particular node is then assigned to one of the
            /// processing elements which assigned the maximal 
            /// priority to the node at hand.
            /// @param[in]  solver  Reference to the solver instance
            /// @param[in]  rank    The rank of the processing element
            /// @param[in]  P   The priority array. The array is
            ///         potentially modified by the routine
            ///         and should not be used afterwards
            void CreateMappingFromPriorities(Solver& solver, int rank, NodalVector<int>& P);

            /// Query the owner of a node
            inline int owner(const Node& nd) const
            {
                return (int )(*this)(nd);
            }

            /// Return true if this processing element is the owner
            /// of the node. Otherwise false is returned
            inline bool bOwn(const Node& nd) const
            {
                return perank == owner(nd);
            }

            /// Return the rank of the processing element
            inline int rank() const
            {
                return perank;
            }

        private:
            /// Pointer to the mesh
            Mesh* mesh;
            /// The rank of the processing element. There is no
            /// necessity that this value is associated to a
            /// communicator. It should however be unique
            int perank;
        };

    }
}


#endif

