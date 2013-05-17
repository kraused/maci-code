
// vi: tabstop=4:expandtab

#ifndef MACI_MPI_CART_COMMUNICATOR_HH_INCLUDED
#define MACI_MPI_CART_COMMUNICATOR_HH_INCLUDED 1

#include "mpi/config.hh"
#include "mpi/Communicator.hh"

namespace mpi {

    /// Cartcommunicator: A cartesian topology communicator
    class Cartcommunicator : public Communicator
    {
    
    public:
        /// Default constructor. This creates an invalid
        /// cartesian communicator
        Cartcommunicator();

        /// Copy constructor
        Cartcommunicator(const Cartcommunicator& other);

        /// Assignment
        Cartcommunicator& operator=(const Cartcommunicator& cart);

        /// Create a new cartesian communicator
        /// @param comm     the old communicator
        /// @param dim      the dimension of the cartesian topology
        /// @param dims     dimensions of the topology
        /// @param periods  periods of the cartesian topology
        /// @param reorder  decides whether ranks may be reordered
        ///         to achieve higher performance
        Cartcommunicator(MPI_Comm comm, int dim, 
                         const int dims[], const int periods[],
                         int reorder = 1);

        /// Create a new cartesian communicator
        /// @param comm     the old communicator
        /// @param dim      the dimension of the cartesian topology
        /// @param dims     dimensions of the topology
        /// @param periods  periods of the cartesian topology
        /// @param reorder  decides whether ranks may be reordered
        ///         to achieve higher performance
        Cartcommunicator(const Communicator& comm, int dim, 
                         const int dims[], const int periods[],
                         int reorder = 1);

        /// Get neighbour rank for one, two or three dimensional
        /// communicators
        /// @param sx   shift in x direction
        /// @param sy   shift in y direction
        /// @param sz   shift in z direction
        /// @returns    the rank of the neighbour obtained from shifting
        ///     my coordinates or MPI_PROC_NULL if there is no
        ///     such neighbour
        int neighbour(int sx, int sy = 0, int sz = 0) const;

    protected:
        /// Dimension of topology
        int dimension;

        /// Initialize
        /// @param comm     the old communicator
        /// @param dim      the dimension of the cartesian topology
        /// @param dims     dimensions of the topology
        /// @param periods  periods of the cartesian topology
        /// @param reorder  decides whether ranks may be reordered
        ///         to achieve higher performance   
        void initialize(MPI_Comm comm, int dim, 
                        const int dims[], const int periods[],
                        int reorder);
    };

}

#endif

