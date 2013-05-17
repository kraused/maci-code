
// vi: tabstop=4:expandtab

#ifndef MD_INTERFACE_WRITER_HH_INCLUDED
#define MD_INTERFACE_WRITER_HH_INCLUDED 1

#include <string>

namespace md
{
    namespace interface
    {

        /// Forwarding
        class System;

        /// Writer: Abstract base class for a writer
        ///   
        /// The writer should be provide a simple interface
        /// to do I/O in a generic way, i.e. without making
        /// use of the special features of the MD implementation.
        /// Of course, if an implementation decides not to
        /// use this functionality, this is also perfectly
        /// fine
        class Writer
        {

        public:
            /// Write the MD system data (particle positions, velocities)
            /// into a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///          class is responsible for adding the correct
            ///          suffix
            /// @param[in]  system  The MD System
            virtual void Write(const std::string& file, System* system) = 0;

            /// Write the particle positions (plus ranks) and one additional
            /// integer array into a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///          class is responsible for adding the correct
            ///          suffix
            /// @param[in]  system  The MD System
            /// @param[in]  ia  the integer array
            /// @param[in]  varname the name of the array
            virtual void WriteArrayInt   (const std::string& file, System* system, int   * a, const std::string& varname) = 0;

            /// Write the particle positions (plus ranks) and one additional
            /// float array into a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///          class is responsible for adding the correct
            ///          suffix
            /// @param[in]  system  The MD System
            /// @param[in]  fa      the float array
            /// @param[in]  varname the name of the array
            virtual void WriteArrayFloat (const std::string& file, System* system, float * a, const std::string& varname) = 0;

            /// Write the particle positions (plus ranks) and one additional
            /// double array into a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///          class is responsible for adding the correct
            ///          suffix
            /// @param[in]  system  The MD System
            /// @param[in]  da      the double array
            /// @param[in]  varname the name of the array
            virtual void WriteArrayDouble(const std::string& file, System* system, double* a, const std::string& varname) = 0;

            /// Factory function for creating a new writer
            /// @param[in]  system  The MD system
            static Writer* New(System* system);

        };

    }
}

#endif

