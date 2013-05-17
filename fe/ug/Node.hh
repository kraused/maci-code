
// vi: tabstop=4:expandtab

#ifndef FE_INTERFACE_UG_NODE_HH_INCLUDED
#define FE_INTERFACE_UG_NODE_HH_INCLUDED 1

#include "ugNodePointer.hh"

namespace fe {
    namespace interface {

        /// Forwarding
        class Element;
        class Solver;
        class Mesh;

        /// A node in a Finite Element mesh. This
        /// class implements the C++ iterator concept
        class Node : public ug::NodePointer<DIM>
        {
            typedef ug::NodePointer<DIM> Base;

        public:
            /// Assignement operator
            inline Node& operator=(const Node& other)
            {
                Base::operator=(other);
                return (*this);
            }

            /// Pre-increment operator
            inline Node& operator++()
            {
                Base::operator++();
                return (*this);
            }

            /// Compare two nodes for equality
            inline bool operator==(const Node& other) const
            {
                return Base::operator==(other);
            }

            /// Compare two nodes for inequality
            inline bool operator!=(const Node& other) const
            {
                return Base::operator!=(other);
            }

            /// Get the coordinates of the node
            inline const double* coords() const
            {
                return getVertex();
            }

            /// Get a unique local identifier for this node.
            /// The returned value is guaranteed to be between
            /// 0 and nnodes()
            inline long locid() const
            {
                return GetId();
            }

            /// The user can attach an additional "user-defined" index to
            /// each node, e.g., to reorder them for better locality.
            /// This is the const version of Index().
            /// @param[in]  solver  The solver instance
            /// @param[in]  nd  The node
            static long  Index(const Solver& solver, const Node& nd)
            {
                return VINDEX(NVECTOR(nd.GetImplementation()));
            }

            /// Return a reference to the user-defined node index. Using
            /// this function, the index can be changed.
                        /// @param[in]  solver  The solver instance
                        /// @param[in]  nd      The node
            static long& Index(Solver& solver, Node& nd)
            {
                return *(long* )&VINDEX(NVECTOR(nd.GetImplementation()));
            }

        private:
            /// Create a Node from an instance of Base
            Node(const Base& other)
            : Base(other)
            {
            }

            /// Make Element a friend so that it can use
            /// this constructor
            friend class Element;

            /// Make the Mesh class a friend so that it can use
            /// the private constructor
            friend class Mesh;

        };

    }
}

#endif

