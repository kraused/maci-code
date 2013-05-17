//
/// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#ifndef MACI_MPI_WINDOW_HH_INCLUDED
#define MACI_MPI_WINDOW_HH_INCLUDED 1

#include "mpi/config.hh"
#include "mpi/Communicator.hh"

namespace mpi {

    /// Window: RMA window
    /// Encapsulates an MPI_Window instance and provides (type safe) access
    /// to the members in the window
    template<typename Tp>
    class Window
    {

        /// Allocate RMA memory
        /// @param nitems        the number of items to allocate
        ///                      note that the allocated memory
        ///                      will contain sizeof(Tp)*nitems
        ///                      bytes
        /// @returns allocate memory or 0
        static Tp *allocate(int nitems)
        {
            if(nitems == 0) {
                return 0;
            } else {
                Tp *mem = 0;
                MPI_Alloc_mem(nitems*sizeof(Tp), MPI_INFO_NULL, &mem);

                CHECK_ALWAYS(mem, "MPI_Alloc_mem failed.");
                std::fill(mem, mem + nitems, (Tp )0);
            
                return mem;
            }
        }

    public:
        /// Create a new window
        /// @param comm the communicator
        /// @param size the window size in units of Tp
        Window(const Communicator& comm, size_t size)
        : win_(MPI_WIN_NULL), mem_(allocate(size)), size_(size*sizeof(Tp)),
          communicator(comm)
        {
            // Get around the "deprecated warning from const char* to char*"
            // warnings
            char str_no_lock[] = "no_lock";
            char str_true[] = "true";

            MPI_Info info;
            MPI_Info_create(&info);
            MPI_Info_set(info, str_no_lock, str_true);

            MPI_Win_create(mem_, size_, sizeof(Tp), info,
                     communicator.getHandle(), &win_);

            MPI_Info_free(&info);
        }

        /// Destroy the instance
        ~Window()
        {
            eval_if_not_finalized( MPI_Win_free(&win_) );
            eval_if_not_finalized( MPI_Free_mem(mem_) );
        }

        /// Fence synchronization
        /// @param assert        program assertion, see the
        ///                      MPI standard for more infos
        void fence(int assert)
        {
            int err = MPI_Win_fence(assert, win_);
            CHECK_ALWAYS(!err, "MPI Call failed");
        }

        /// Put; Compare with the MPI standard for the arguments
        /// (e.g. MPI-2.1, p. 326)
        void put(void *oaddr, int ocnt, MPI_Datatype odt,
             int trank, MPI_Aint tdisp, int tcnt, MPI_Datatype tdt)
        {
            int err = MPI_Put(oaddr, ocnt, odt, trank, tdisp, tcnt, tdt, win_);
            CHECK_ALWAYS(!err, "MPI Call failed");
        }

        /// Get access to the memory
        /// @param k     offset relative to the base
        /// @returns     entry of type Tp
        inline Tp operator[](int k) const
        {
            CHECK_PRECOND_ALWAYS(k >= 0 && 
                    (MPI_Aint )(k*sizeof(Tp)) < size_);
            return mem_[k];
        }

        /// Get access to the memory
        /// @param k     offset relative to the base
        /// @returns     entry of type Tp
        inline Tp& operator[](int k)
        {
            CHECK_PRECOND_ALWAYS(k >= 0 && 
                    (MPI_Aint )(k*sizeof(Tp)) < size_);
            return mem_[k];
        }

        
    private:
        //// The window handle
        MPI_Win win_;
        //// The memory for the window
        Tp *mem_;
        //// Size of window in bytes
        MPI_Aint size_;
        /// The communicator
        Communicator communicator;
    };

}

#endif

