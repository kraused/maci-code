
// vi: tabstop=4:expandtab

#include "Logging.hh"
#include "utils/strprintf.hh"
#include "mpi/Communicator.hh"
#include "config.hh"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif


static double wtime()
{
        struct timeval tv;
        gettimeofday(&tv, NULL);

        return tv.tv_sec + (tv.tv_usec/1e6);
}


/// Static log instance
static Log log;

Log& Log::Instance()
{
    return log;
}

Log::Log()
{
    rank  = mpi::Communicator(MPI_COMM_WORLD).myrank();
    logfo = fopen(utils::strprintf("log-%d.txt", rank).c_str(), "w");

#ifdef HAVE_UNISTD_H
    /* This is a dirty way to deal with the printf()'s in the
       code */
    dup2(fileno(logfo), STDOUT_FILENO);
    dup2(fileno(logfo), STDERR_FILENO);

    /* Disable buffering on stdout and stderr. */
    fflush(stdout);
    setbuf(stdout, NULL);
    fflush(stderr);
    setbuf(stderr, NULL);
#endif

    tstart = wtime();
}

Log::~Log()
{
    fclose(logfo);
}

void Log::write(const std::string& text)
{
    fprintf(logfo, "%s", text.c_str()); /// Funny: On OS X the gcc compiler
                        /// warns me if I use fprintf(..., text.c_str())
                        /// without the "%s" format. Most likely
                        /// because this version is a security
                        /// problem if the text comes from (potentially
                        /// malicious users)
    fflush (logfo);
}

void Log::write(const char* fmt, ...)
{
    va_list  args;
    va_start(args, fmt);

    write(utils::strprintf(" %-9.3f: ", wtime()-tstart) + utils::strvprintf(fmt, args));

    va_end  (args);
}

