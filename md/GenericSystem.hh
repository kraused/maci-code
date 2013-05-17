
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_GENERIC_GENERIC_SYSTEM_HH_INCLUDED
#define MD_INTERFACE_GENERIC_GENERIC_SYSTEM_HH_INCLUDED 1

namespace md {
    namespace interface {

        /// Implementation independent system class which serves as a
        /// base class for all implementations
        template<typename ImplClass, typename Float>
        class GenericSystem
        {

        public:
            /// Access types
            enum {
                INVALID     = 0x0000,
                /// Read/Write access to positions
                RD_POS      = 0x0001,
                WR_POS      = 0x0002,
                /// Read/Write access to velocities
                RD_VEL      = 0x0004,
                WR_VEL      = 0x0008,
                /// Read/Write access to reference positions
                RD_REFP     = 0x0010,
                WR_REFP     = 0x0020,
                /// Read/Write access to forces
                RD_FORCE    = 0x0040,
                WR_FORCE    = 0x0080,
                /// Read/Write access to masses
                RD_MASS     = 0x0100,
                WR_MASS     = 0x0200,
                /// Read/Write access to piggyback data
                RD_PIGGYBACK    = 0x0400,
                WR_PIGGYBACK    = 0x0800,
                /// Read uids of particles (no write permitted)
                /// This one is by default included if any of
                /// the other flags is given
                RD_UID      = 0x1000
            };

            /// Constructor
            /// @param[in]  Imp the implementation instance
            GenericSystem(ImplClass &Imp)
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

            /// Floating point type used in the MD code to store
            /// positions, velocities and forces
            typedef Float     fp_t;

        private:
            /// Reference to the implementation. The type is defined
            /// by the derived class.
            ImplClass& Imp_;

        };

    }
}

#endif

