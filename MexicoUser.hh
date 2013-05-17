
// vi: tabstop=4:expandtab

#ifndef MACI_COMMLIB_USER_HH_INCLUDED
#define MACI_COMMLIB_USER_HH_INCLUDED 1

#include <valarray>
#include "mexico.hpp"
#include "ParticleList.hh"
#include "mpi/Communicator.hh"


/// MexicoUser: Base class for all classes that use the
///              mexico layer for (irregular) communcation
class MexicoUser
{

public:
    /// Create a new instance
    MexicoUser();

    /// The Tp functor class must implement the following functions
    /// - static int i_cnt()                                           : Returns the count of data items to be
    ///                                                                  send to each worker
    /// - static int o_cnt()                                           : Returns the count of data items to
    ///                                                                  receive from each worker
    /// - int num_i_worker(System*, const ParticleHandle&) const       : Returns the number of workers to
    ///                                                                  which the data of this particle
    ///                                                                  should be sent
    /// - int num_o_worker(System*, const ParticleHandle&) const       : Returns the number of workers to
    ///                                                                  receive data from
    /// - int i_worker(System*, const ParticleHandle&, int) const      : Returns the rank of the k-th
    ///                                                                  worker to send data to
    /// - int o_worker(System*, const ParticleHandle&, int) const      : Returns the rank of the k-th
    ///                                                                  worker to receive data from
    /// - int i_offset(System*, const ParticleHandle&, int) const      : Returns the offset of the data for
    ///                                                                  the k-th worker on the remote side
    /// - int o_offset(System*, const ParticleHandle&, int) const      : Returns the offset of the data to
    ///                                                                  be grepped from the k-th worker in
    ///                                                                  the remote buffer
    /// - void pack(System*, const ParticleHandle&, int, double*) const: Pack the values for the k-th worker
    /// - void scat(System*, ParticleHandle&, int, double*) const      : Scatter the values received from
    ///                                                                  the k-th worker
    
    /// Pack the values in the communication buffer and upate i_cnt, o_cnt, ...
    template<typename Tp>
    void pack(ParticleList list, const Tp& f);

    /// Scatter the computed results back
    template<typename Tp>
    void scat(ParticleList list, const Tp& f);

    /// Call the cli->exec() function
    void execute_cli();

    /// Gather the worker array. Each processing element decides whether
    /// or not it will be a worker and announces this to all other processing
    /// elements. The function is collective.
    /// @param[in]  comm        The communicator used for the communication
    /// @parma[in]  i_am_worker 1 if the processing element is a worker and zero
    ///                         otherwise
    void gather_worker(const mpi::Communicator& comm, int i_am_worker);

    /// Open the configuration file for this class. The function first looks for
    /// the file in the current directory and then in the standard share folder.
    /// This allows us to perform
    FILE* open_config_file(const std::string& filename);

    /// Create the cli instance. This function assumes that worker is initialized
    /// and job is either NULL or points to a mexico::Job instance.
    /// @param[in]  filename    The name of the configuration file. This is directly
    ///                         passed to open_config_file().
    void create_cli(mpi::Communicator& comm, const std::string& filename);

    /// The derived classes are responsible for initializing
    /// and deleting the job and cli instance. This is simpler
    /// than providing both as an argument to the constructor
public:
    /// The job
    mexico::Job* job;
    /// The mexico instance
    mexico::Instance* cli;
    /// List of workers
    std::valarray<int> worker;

    /// protected so that derived classes which do not call pack() can manually
    /// set i_cnt, o_cnt (must be set on all processing elements to the same value).
protected:
    /// Input and output count
    int i_cnt, o_cnt;
private:
    /// Input and output number of values
    int i_num_vals, o_num_vals;
    /// Input and output maximal worker number
    int i_max_worker_per_val, o_max_worker_per_val;
    /// Input and output buffer
    double* i_buf;
    double* o_buf;
    /// Input and output worker arrays
    int* i_worker;
    int* o_worker;
    /// Input and output offsets array
    int* i_offset;
    int* o_offset;

};


template<typename Tp>
void MexicoUser::pack(ParticleList list, const Tp& f)
{
    long ii;
    int k;
    md::interface::System* system = gcpe()->GetMMSystem();  /// TODO: Shouldn't use gcpe() anymore. On
                                                            ///       the other hand, MexicoUser should
                                                            ///       not derive from Pointers

    i_cnt = Tp::i_cnt();
    o_cnt = Tp::o_cnt();

    i_num_vals = o_num_vals = list.size();

    i_max_worker_per_val = 0;
    o_max_worker_per_val = 0;

    for(ParticleList::ConstIterator i = list.begin(); i != list.end(); ++i)
    {
        i_max_worker_per_val = std::max(i_max_worker_per_val, f.num_i_worker(system, *i));
        o_max_worker_per_val = std::max(o_max_worker_per_val, f.num_o_worker(system, *i));
    }

    CHECK_ALWAYS(i_cnt > 0 and o_cnt > 0, "Invalid cnt");

    i_buf = (double* )realloc(i_buf, i_cnt*i_num_vals*sizeof(double));
    o_buf = (double* )realloc(o_buf, o_cnt*o_num_vals*o_max_worker_per_val*sizeof(double));

    i_worker = (int* )realloc(i_worker, i_num_vals*i_max_worker_per_val*sizeof(int));
    o_worker = (int* )realloc(o_worker, o_num_vals*o_max_worker_per_val*sizeof(int));

    /// -1 indicates an invalid entry. These are ignored by mexico
    std::fill(i_worker, i_worker + i_num_vals*i_max_worker_per_val, -1);
    std::fill(o_worker, o_worker + o_num_vals*o_max_worker_per_val, -1);

    i_offset = (int* )realloc(i_offset, i_num_vals*i_max_worker_per_val*sizeof(int));
    o_offset = (int* )realloc(o_offset, o_num_vals*o_max_worker_per_val*sizeof(int));
    
    std::fill(i_offset, i_offset + i_num_vals*i_max_worker_per_val, -1);
    std::fill(o_offset, o_offset + o_num_vals*o_max_worker_per_val, -1);

    /// Note: mexico wants to have i_worker and i_offsets in column-major ordering
    ///       (and it also returns o_buf in column major ordering). This is bad for
    ///       the gather and scatter operations which write to strided values but
    ///       is better for the communication library because values which will be
    ///       send to close-by memory locations (in a global-view) are close to each
    ///       other (e.g., values which are send to the same
    ///       worker are close to each other in the buffer).

    ii = 0;
    for(ParticleList::ConstIterator i = list.begin(); i != list.end(); ++i)
    {
        for(k = 0; k < f.num_i_worker(system, *i); ++k)
        {
            i_worker[ii + i_num_vals*k] = f.i_worker(system, *i, k);
            i_offset[ii + i_num_vals*k] = f.i_offset(system, *i, k);

            CHECK_DEBUG(i_worker[ii + i_num_vals*k] >= 0, 0);
        }

        for(k = 0; k < f.num_o_worker(system, *i); ++k)
        {
            o_worker[ii + o_num_vals*k] = f.o_worker(system, *i, k);
            o_offset[ii + o_num_vals*k] = f.o_offset(system, *i, k);

            CHECK_DEBUG(o_worker[ii + o_num_vals*k] >= 0, 0);
        }

        f.pack(system, *i, &i_buf[i_cnt*(ii++)]);
    }

    CHECK_ALWAYS(ii == i_num_vals, 0);
    CHECK_ALWAYS(ii == o_num_vals, 0);
}

template<typename Tp>
void MexicoUser::scat(ParticleList list, const Tp& f)
{
    long ii;
    int k;
    md::interface::System* system = gcpe()->GetMMSystem();  /// TODO: Shouldn't use gcpe() anymore. On
                                                            ///       the other hand, MexicoUser should
                                                            ///       not derive from Pointers

    ii = 0;
    for(ParticleList::Iterator i = list.begin(); i != list.end(); ++i)
    {
        for(k = 0; k < f.num_o_worker(system, *i); ++k)
            f.scat(system, *i, k, &o_buf[o_cnt*(ii + o_num_vals*k)]);

        ++ii;
    }

    CHECK_ALWAYS(ii == o_num_vals, 0);
}

#endif

