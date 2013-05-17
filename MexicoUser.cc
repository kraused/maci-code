
// vi: tabstop=4:expandtab

#include <numeric>

#include "MexicoUser.hh"
#include "Logging.hh"
#include "config.hh"
#include "DesignByContract.hh"


MexicoUser::MexicoUser()
: i_buf(0), o_buf(0), i_worker(0), o_worker(0), i_offset(0), o_offset(0)
{
    /// If we don't call pack() we don't want
    /// to send anything
    i_cnt = o_cnt = 0;
    i_num_vals = o_num_vals = 0;
    i_max_worker_per_val = o_max_worker_per_val = 0;
}

void MexicoUser::execute_cli()
{
    /// i_cnt and o_cnt must be consistent between the different
    /// processing elements, so a value of 0 is not possible!
    CHECK_PRECOND_ALWAYS(0 != i_cnt and 0 != o_cnt);

    cli->exec(i_buf, i_cnt, MPI_DOUBLE, i_num_vals, i_max_worker_per_val, i_worker, i_offset,
              o_buf, o_cnt, MPI_DOUBLE, o_num_vals, o_max_worker_per_val, o_worker, o_offset);
}

void MexicoUser::gather_worker(const mpi::Communicator& comm, int i_am_worker)
{
    int i, k, *t, N;

    CHECK_PRECOND_DEBUG(0 == i_am_worker || 1 == i_am_worker);

    t = new int[comm.size()]();
    t[comm.myrank()] = i_am_worker;

    comm.allreduce(MPI_IN_PLACE, t, comm.size(), MPI_INT, MPI_SUM);

    N = std::accumulate(t, t + comm.size(), 0);
    CHECK_ALWAYS(N >= 0, 0);
    CHECK_ALWAYS(N <= comm.size(), 0);

    worker.resize(N);

    k = i = 0;
    while(i < N and k < comm.size())
    {
        if(0 != t[k])
            worker[i++] = k;
        ++k;
    }

    CHECK_POSTCOND_ALWAYS(i == N);

    delete[] t;
}

FILE* MexicoUser::open_config_file(const std::string& filename)
{
    FILE* fi;

    fi = fopen(filename.c_str(), "r");
    if(NULL != fi)
    {
        Log::Instance().write("Using local configuration file \"%s\"\n", filename.c_str());
    }
    else
    {
        /// Couldn't find the file in the current directory. Read the one 
        /// from the share folder
        std::string default_file = std::string(SHARE_FOLDER) + "/" + filename;
        fi = fopen(default_file.c_str(), "r");

        CHECK_ALWAYS(NULL != fi, "Check your installation!");
        Log::Instance().write("Using default configuration file \"%s\"\n", default_file.c_str());
    }

    return fi;
}

void MexicoUser::create_cli(mpi::Communicator& comm, const std::string& filename)
{
    FILE* fi;

    fi  = open_config_file(filename);
    cli = new mexico::Instance(comm.getHandle(), worker.size(), &worker[0], job, fi);

    fclose(fi);
}

