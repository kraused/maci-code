
// vi: tabstop=4:expandtab

#include "MonitorResidual.hh"
#include "DesignByContract.hh"
#include "Logging.hh"
#include "TimeIntegrator.hh"
#include "DAEqCorrector.hh"


using namespace plugins;


/* -----------------------------------------------------------------------------
 * Class member functions
 * -------------------------------------------------------------------------- */

MonitorResidual::MonitorResidual(Maci* app, const char* outfile)
: Plugin(app, "MonitorResidual"), outfile(outfile), step(0), fo(0), comp(0)
{
}

MonitorResidual::MonitorResidual(const MonitorResidual &o)
: Plugin(o), outfile(o.outfile), step(o.step), fo(0), comp(0)
{
    if(o.fo)
        fo = fopen(outfile.c_str(), "w");
}

MonitorResidual* MonitorResidual::clone() const
{
    return new MonitorResidual(*this);
}

void MonitorResidual::pre_integration()
{
    if(std::string("-") != outfile)
        fo = fopen(outfile.c_str(), "w");
}

void MonitorResidual::post_step()
{
    double nrm2u, nrm2v;

    if(!comp)
    {
        CHECK_PRECOND_ALWAYS(integrator->corrector);
        comp = integrator->corrector->NewResidualNormComputer();
    }

    nrm2u = comp->Nrm2U();
    nrm2v = comp->Nrm2V();

    /// Increment the step before printing because
    /// this is post_step()
    ++step;

    if(fo)
        fprintf(fo, "%ld\t%e\t%e\n", step, nrm2u, nrm2v);
    else
        Log::Instance().write("MonitorResidual: %-6ld\t%e\t%e\n", step, nrm2u, nrm2v);
}

void MonitorResidual::post_integration()
{
    if(fo)
        fclose(fo);
}

