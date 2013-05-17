
// vi: tabstop=4:expandtab

#ifndef FE_INTERFACE_UG_MESH_HH_INCLUDED
#define FE_INTERFACE_UG_MESH_HH_INCLUDED 1

#include "fe/ug/Node.hh"
#include "fe/ug/Element.hh"

#include "ugMesh.hh"

namespace fe {
    namespace interface {

        /// Forwarding
        class Solver;
        class NodeDistribution;
        template<typename Tp, int N>
        class NodalVector;

        /// Interface to the Finite Element Mesh 
        class Mesh
        {
            typedef ug::Mesh<DIM> Base;

        public:
            /// Returns the number of nodes in the local part
            /// of the mesh. This includes shared nodes, i.e.~
            /// the sum of the value from all processes does
            /// exceed the total number of nodes in the mesh
            inline long nnodes() const
            {
                return base->GetNumNodes();
            }
            
            /// Return handle for the first node in the
            /// local part of the mesh
            inline Node NodesBegin() const
            {
                return base->NodesBegin();
            }

            /// Return handle for the end of the node list
            inline Node NodesEnd() const
            {
                return base->NodesEnd();
            }

            /// Returns the number of elements in the local
            /// part of the mesh. This does not include halo
            /// cells (if use by the code at all), i.e. the
            /// sum of these values from all processes equals
            /// the number of elements in the mesh
            inline long nelements() const
            {
                return base->GetNumElements();
            }

            /// Return handle for the first element in the
            /// local part of the mesh
            inline Element ElementsBegin() const
            {
                return base->ElementsBegin();
            }

            /// Return handle for the end of the element list
            inline Element ElementsEnd() const
            {
                return base->ElementsEnd();
            }

            /// Return the lower and upper corner of the bounding box
            /// of the local mesh part
            void BoundingBox(double* lower, double* upper) const;

            /// Compute the maximal diameter of the elements in the local
            /// mesh part
            double MaxElementDiameter() const;

        private:
            /// We store a pointer to an instance of the base class
            /// rather than deriving because the UG interface doesn't
            /// allow more than one instances of Mesh and also we
            /// don't want to copy this.
            Base* base;

            /// Private constructor
            Mesh(Base* mesh)
            : base(mesh)
            {
            }

            /// Make Solver a friend so that it can call the private
            /// constructor
            friend class Solver;

            /// NodalVector needs to be a friend, too, so that we can
            /// instantiate fe::ug::NodalVector instances
            template<typename Tp, int N>
            friend class NodalVector;
        };

    }
}

#endif

