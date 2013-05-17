
// vi: tabstop=4:expandtab

#ifndef MACI_FE_INTERFACE_PVO_WRITER_HH_INCLUDED
#define MACI_FE_INTERFACE_PVO_WRITER_HH_INCLUDED 1

#include <string>
#include <stdint.h>
#include <pvo.h>

#include "fe/Writer.hh"

namespace fe
{
    namespace interface
    {

        /// Forwarding
        class Solver;

        /// PvoWriter: Implementation of the Writer
        ///   interface using the PVO library
        class PvoWriter : public Writer
        {
    
        public:
            /// Create a new instance. The memory allocations
            /// are all done in the constructor
            PvoWriter(Solver* solver);
    
            /// Destructor.
            ~PvoWriter();
    
            /// Write the mesh data together with displacement
            /// values and velocities to a file
            /// @param[in]  file    The prefix of the output file. The derived
            ///      class is responsible for adding the correct
            ///      suffix
            /// @param[in]  solver  Pointer to the Solver instance
            void Write(const std::string& file, Solver* solver);

            /// Write the mesh connectivity, ranks and one additional integer
            /// array (nodal) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            void WriteNodalArrayInt   (const std::string& file, Solver* solver, int   * a, const std::string& varname);

            /// Write the mesh connectivity, ranks and one additional float
            /// array (nodal) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            void WriteNodalArrayFloat (const std::string& file, Solver* solver, float * a, const std::string& varname);

            /// Write the mesh connectivity, ranks and one additional double
            /// array (nodal) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            void WriteNodalArrayDouble(const std::string& file, Solver* solver, double* a, const std::string& varname);

            /// Write the mesh connectivity, ranks and one additional integer
            /// array (element) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            void WriteElementArrayInt   (const std::string& file, Solver* solver, int   * a, const std::string& varname);

            /// Write the mesh connectivity, ranks and one additional integer
            /// array (element) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            void WriteElementArrayFloat (const std::string& file, Solver* solver, float * a, const std::string& varname);

            /// Write the mesh connectivity, ranks and one additional integer
            /// array (element) into a file
            /// @param[in]  file    The prefix of the output file
            /// @param[in]  solver  Pointer to the Solver instance
            /// @param[in]  a       the array
            /// @param[in]  varname the name of the array
            void WriteElementArrayDouble(const std::string& file, Solver* solver, double* a, const std::string& varname);

        private:
            /// Output file. This is an opaque type
            pvo_vtu_file_t fo;
            /// The PVO cookie
            pvo_cookie_t cookie;
            /// The mesh point coordinates
            pvo_float3_t* pts;
            /// The mesh connectivity stored as a CSR graph
            int* cia;
            int* cja;
            /// The cell types
            uint8_t* types;
            /// Contiguous buffers for displacement and
            /// velocities
            double* buf_u;
            double* buf_v;
            /// Ranks array. This allows for identifying which
            /// cells are located on which processing element
            int* ranks;
        };

    }
}

#endif

