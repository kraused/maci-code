
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_PVO_WRITER_HH_INCLUDED
#define MD_INTERFACE_PVO_WRITER_HH_INCLUDED 1

#include <string>
#include <pvo.h>

#include "md/Writer.hh"

namespace md
{
    namespace interface
    {

        /// Forwarding
        class System;

        /// PvoWriter: Implements the Writer class using the
        ///     PVO library
        class PvoWriter : public Writer
        {

        public:
            /// Create a new instance
            PvoWriter(System* system);

            /// Destructor.
            ~PvoWriter();
            
            /// Write the MD system data (particle positions, velocities)
            /// into a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///                  class is responsible for adding the correct
            ///                  suffix
            /// @param[in]  system  The MD System
            void Write(const std::string& file, System* system);

            /// Write the particle positions (plus ranks) and one additional
            /// integer array into a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///                  class is responsible for adding the correct
            ///                  suffix
            /// @param[in]  system  The MD System
            /// @param[in]  ia      the integer array
            /// @param[in]  varname the name of the array
            void WriteArrayInt   (const std::string& file, System* system, int   * a, const std::string& varname);

            /// Write the particle positions (plus ranks) and one additional
            /// float array into a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///                  class is responsible for adding the correct
            ///                  suffix
            /// @param[in]  system  The MD System
            /// @param[in]  fa      the float array
            /// @param[in]  varname the name of the array
            void WriteArrayFloat (const std::string& file, System* system, float * a, const std::string& varname);

            /// Write the particle positions (plus ranks) and one additional
            /// double array into a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///                  class is responsible for adding the correct
            ///                  suffix
            /// @param[in]  system  The MD System
            /// @param[in]  da      the double array
            /// @param[in]  varname the name of the array
            void WriteArrayDouble(const std::string& file, System* system, double* sa, const std::string& varname);

        private:
            /// Output file. This is an opaque type
            pvo_vtp_file_t fo;
            /// The PVO cookie
            pvo_cookie_t cookie;

        };

    }
}

#endif

