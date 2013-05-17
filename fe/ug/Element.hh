
// vi: tabstop=4:expandtab

#ifndef FE_INTERFACE_UG_ELEMENT_HH_INCLUDED
#define FE_INTERFACE_UG_ELEMENT_HH_INCLUDED 1

#include <ostream>

#include "fe/ug/Node.hh"

#include "ugElementPointer.hh"

namespace fe {
    namespace interface {

        /// Forwarding
        class Mesh;

        /// The maximal number of corners that the Finite Element
        /// code supports.
        static const int MAX_CORNERS_OF_ELEMENT = 8;

        /// A cell in a Finite Element mesh. This
        /// class implements the C++ iterator concept
        class Element : protected ug::ElementPointer<DIM>
        {
            typedef ug::ElementPointer<DIM> Base;

        public:
            /// Assignement operator
            inline Element& operator=(const Element& other)
            {
                Base::operator=(other);
                return (*this);
            }

            /// Pre-increment operator
            inline Element& operator++()
            {
                Base::operator++();
                return (*this);
            }

            /// Compare two nodes for equality
            inline bool operator==(const Element& other)
            {
                return Base::operator==(other);
            }

            /// Compare two nodes for inequality
            inline bool operator!=(const Element& other)
            {
                return Base::operator!=(other);
            }

            /// Return the number of corners
            inline int ncorners() const
            {
                return GetNumCorners();
            }

            /// Return the i-th corner node
            inline Node corner(int i) const
            {
                return Node(Base::corner(i));
            }

            /// Return the lower and upper coordinates of the
            /// bounding box of the element. If the FE code
            /// itself does not provide this information we can
            /// easily compute them with a loop over all corners
            void BoundingBox(double* lower, double* upper) const;

        public:
            /// Create an Element from an instance of Base
            Element(const Base& other)
            : Base(other)
            {
            }

            /// Make the Mesh class a friend so that it can use
            /// the private constructor
            friend class Mesh;


        };

        /// Print an element to stdout. This is very usefull for
        /// debugging
        inline std::ostream& operator<<(std::ostream& out, const Element& el)
        {
            int i;

            out << "{ ncorners: " << el.ncorners() << ", corners: [ ";
            for(i = 0; i < el.ncorners(); ++i)
            {
                out << "[ " << el.corner(i).coords()[0];
                out << ", " << el.corner(i).coords()[1];
#if 2 == DIM
                out << ", " << 0.0;
#else
                out << ", " << el.corner(i).coords()[2];
#endif
                out << " ]";
            }
            out << " ] }";

            return out;
        }

    }
}

#endif

