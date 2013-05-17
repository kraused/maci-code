
// vi: tabstop=4:expandtab

#ifndef MACI_FE_INTERFACE_SRC_WRITER_HH_INCLUDED
#define MACI_FE_INTERFACE_SRC_WRITER_HH_INCLUDED 1
        
#include <string>

namespace fe
{
    namespace interface
    {

        /// Forwarding
        class Solver;

        /// Writer: Abstract base class for writer implemenation.
        ///
        /// Writer defines the interface that each implementation must
        /// provide. Doing it this way allows us to switch implementations
        /// of the I/O functionality without any pain. Moreover, in the
        /// future the choice of the output writer could be done at
        /// runtime based on the user input (e.g., to allow for writing
        /// in different formats)
        class Writer
        {
    
        public:
            /// Write the mesh data together with displacement
            /// values and velocities to a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///         class is responsible for adding the correct
            ///         suffix
            /// @param[in]  solver  Pointer to the Solver instance
            virtual void Write(const std::string& file, Solver* solver) = 0;


            /// Write the mesh connectivity, ranks and one additional integer
            /// array (nodal) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a   the array
            /// @param[in]  varname the name of the array
            virtual void WriteNodalArrayInt   (const std::string& file, Solver* solver, int   * a, const std::string& varname) = 0;

            /// Write the mesh connectivity, ranks and one additional float
            /// array (nodal) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            virtual void WriteNodalArrayFloat (const std::string& file, Solver* solver, float * a, const std::string& varname) = 0;

            /// Write the mesh connectivity, ranks and one additional double
            /// array (nodal) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            virtual void WriteNodalArrayDouble(const std::string& file, Solver* solver, double* a, const std::string& varname) = 0;

            /// Write the mesh connectivity, ranks and one additional integer
            /// array (element) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            virtual void WriteElementArrayInt   (const std::string& file, Solver* solver, int   * a, const std::string& varname) = 0;

            /// Write the mesh connectivity, ranks and one additional integer
            /// array (element) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            virtual void WriteElementArrayFloat (const std::string& file, Solver* solver, float * a, const std::string& varname) = 0;

            /// Write the mesh connectivity, ranks and one additional integer
            /// array (element) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            virtual void WriteElementArrayDouble(const std::string& file, Solver* solver, double* a, const std::string& varname) = 0;

            /// Factory function for creating a new writer
            /// @param[in]  solver  Pointer to the Solver instance
            static Writer* New(Solver* solver);

        };

    }
}

#endif

