
// vi: tabstop=4:expandtab

#include "ShowProgress.hh"
#include "DesignByContract.hh"
#include "ProcElement.hh"
#include "Maci.hh"
#include "Logging.hh"
#include "DAEqCorrector.hh"
#include "TimeIntegrator.hh"
#include "DampingMethod.hh"

using namespace plugins;

/* -----------------------------------------------------------------------------
 * Class member functions
 * -------------------------------------------------------------------------- */

ShowProgress::ShowProgress(Maci* app, int themask)
: Plugin(app, "ShowProgress"), mask(themask), step(0)
{
}

ShowProgress::ShowProgress(const ShowProgress &o)
: Plugin(o), mask(o.mask), step(o.step)
{
}

ShowProgress* ShowProgress::clone() const
{
    return new ShowProgress(*this);
}

void ShowProgress::pre_integration()
{
    Log::Instance().write("Starting time integration.\n");

    time_step  = 0;
    time_force = 0;
}

void ShowProgress::post_integration()
{
    Log::Instance().write("Finished time integration.\n");
}

void ShowProgress::pre_step()
{
    t0 = MPI_Wtime();
}

void ShowProgress::post_step()
{
    double time_damper, time_corrector;

    t1 = MPI_Wtime();
    time_step += t1 - t0;

    /// Note: pre-increment of step
    if(0 == (++step)%mask)
    {
        if(PROC_MD == procel->mytype)
            time_damper = integrator->damper->time_spend/(double )mask;
        else
            time_damper = 0.0;
        time_corrector = integrator->corrector->time_spend/(double )mask;
        time_step = time_step/(double )mask;
        time_force = time_force/(double )mask;

        Log::Instance().write("step %6d: Avg. time per step %.3f (%.3f%% force, %.3f%% damper, %.3f%% corrector)\n",
                              step, time_step, time_force*100.0/time_step, time_damper*100.0/time_step, time_corrector*100.0/time_step);

        /// We need to reset the time
        if(PROC_MD == procel->mytype)
                integrator->damper->time_spend = 0.0;
        integrator->corrector->time_spend = 0.0;

        time_step = 0;
        time_force = 0;
    }
}

void ShowProgress::pre_force()
{
    t2 = MPI_Wtime();
}

void ShowProgress::post_force()
{
    t3 = MPI_Wtime();
    time_force += t3 - t2;
}

