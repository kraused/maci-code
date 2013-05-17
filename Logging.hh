
// vi: tabstop=4:expandtab

#ifndef MACI_LOGGING_HH_INCLUDED
#define MACI_LOGGING_HH_INCLUDED 1

#include <stdio.h>
#include <string>

/// Log: Represents a log file
/// 
/// The Log class implements (parallel) logging
/// Currently, it simply opens one file for each
/// process because there is no way to implement
/// locking.
class Log
{

public:
    /// Factory function. Returns a reference
    /// to the one-and-only instance of Log
    static Log& Instance();

    /// Constructor. Since there should be only
    /// on instance of Log this should not be called
    /// by users
    Log();

    /// Destructor
    ~Log();

    /// Print to the logfile
    void write(const std::string& text);

    /// Print to the logfile. The function supports
    /// writef-like syntax
    void write(const char* fmt, ...);

private:
    /// The logfile
    FILE*   logfo;
    /// The rank of the process.
    int     rank;
    /// The start time of the internal counter
    double  tstart;
};

#endif

