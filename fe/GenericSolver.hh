
// vi: tabstop=4:expandtab

#ifndef FE_GENERIC_GENERIC_SOLVER_HH_INCLUDED
#define FE_GENERIC_GENERIC_SOLVER_HH_INCLUDED 1

namespace fe {
    namespace interface {

        /// Implementation independent system class which serves as a
        /// base class for all Solver implementations
        ///
        /// For the solver we use the same pattern as employed in the
        /// MD System implementation: Access to the values of the relevant
        /// state variables is only permitted if an epoch was started
        /// and access for this quantity was requested. This allows for
        /// implementing the coupling in heterogeneous environments.
        ///
        template<typename ImplClass>
        class GenericSolver
        {

        public:

            /// Access types
            enum
            {
                INVALID = 0x0000,
                /// Read/Write access to the displacements
                RD_DISP = 0x0001,
                WR_DISP = 0x0002,
                /// Read/Write access to the velocities
                RD_VEL  = 0x0004,
                WR_VEL  = 0x0008
            };

            /// Constructor
            /// @param[in]  Imp the implementation instance
            GenericSolver(ImplClass &Imp)
            : Imp_(Imp)
            {
            }
            
            /// Get a pointer to the implementation. 
            /// This is only intended for internal use in the 
            /// interface implementation
            inline ImplClass *GetImplementation()
            {
                return &Imp_;
            }

            /// Get a pointer to the implementation (const version).
            /// This is only intended for internal use in the 
            /// interface implementation
            inline const ImplClass *GetImplementation() const
            {
                return &Imp_;
            }

            /// Implementation type
            typedef ImplClass Implementation;

        private:
            /// Reference to the implementation. The type is defined
            /// by the derived class.
            ImplClass& Imp_;

        };

    }
}

#endif

