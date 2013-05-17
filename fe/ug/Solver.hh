
// vi: tabstop=4:expandtab

#ifndef MACI_FE_INTERFACE_UG_SOLVER_HH_INCLUDED
#define MACI_FE_INTERFACE_UG_SOLVER_HH_INCLUDED 1

#include <mpi.h>

#include "fe/GenericSolver.hh"
#include "fe/Writer.hh"

#include "fe/ug/Mesh.hh"
#include "fe/ug/NodeDistributionMap.hh"

#include "fe/ug/src/SolverImplementation.hh"

namespace fe {
    namespace interface {

        /// Solver: Bundles the various parts of the FE simulation
        ///         and provides a unified interface
        class Solver : public GenericSolver<UG_IF::SolverImplementation>
        {
            typedef GenericSolver<UG_IF::SolverImplementation> Base;

        public:

            /// Create a new instance. The implementation is
            /// <b>not</b> owned by the system class
            Solver(Implementation& Imp);

            ~Solver();

            /// Write the mesh together with the important state variables
            /// into a file
            /// @param[in]  file    The name of the output
            ///              file without suffix.
            void Write(const std::string& file);

            /// Starts an access epoch.
            /// @param[in]  type    defines the access pattern of the
            ///         epoch. The behavior is undefined if
            ///         other quantities (not requested via
            ///         type) are accessed
            void AccessBegin(int type);

            /// Ends an access epoch. This is a collective call,
            /// i.e., all finite element processing elements must
            /// call the function to guarantee progress
            void AccessEnd();

            /// Return a pointer to the FE mesh (const version)
            inline const Mesh* GetMesh() const
            {
                return &mesh;
            }

            /// Return a pointer to the FE mesh
            inline Mesh* GetMesh()
            {
                return &mesh;
            }

            /// Return pointer to the NodeDistributionMap instance (const version)
            inline const NodeDistributionMap* GetNodeDistribMap() const
            {
                return &distrib_map;
            }

            /// Return pointer to the NodeDistributionMap instance
            inline NodeDistributionMap* GetNodeDistribMap()
            {
                return &distrib_map;
            }

            /// Return the communicator used by the FE toolkit
            MPI_Comm GetCommunicator();

        private:
            /// Fascade/Wrapper around the Mesh implementation in the
            /// toolbox
            Mesh mesh;

            /// The type of the current epoche. We need store this so
            /// that we know what to do in AccessEnd()
            int epoch_type;

            /// Node distribution manager
            NodeDistributionMap distrib_map;

        private:
            /// I/O writer instance.
            Writer* writer;
        
        };

    }
}

#endif

