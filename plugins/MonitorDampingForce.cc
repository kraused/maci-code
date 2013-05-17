
// vi: tabstop=4:expandtab

#include "MonitorDampingForce.hh"
#include "DesignByContract.hh"
#include "Logging.hh"
#include "TimeIntegrator.hh"
#include "DampingMethod.hh"


using namespace plugins;


/* -----------------------------------------------------------------------------
 * Class member functions
 * -------------------------------------------------------------------------- */

MonitorDampingForce::MonitorDampingForce(Maci* app, const char* outfile)
: Plugin(app, "MonitorDampingForce"), outfile(outfile), step(0), fo(NULL)
{
}

MonitorDampingForce::MonitorDampingForce(const MonitorDampingForce &o)
: Plugin(o), outfile(o.outfile), step(o.step), fo(NULL)
{
    if(o.fo)
        fo = fopen(outfile.c_str(), "w");
}

MonitorDampingForce* MonitorDampingForce::clone() const
{
    return new MonitorDampingForce(*this);
}

void MonitorDampingForce::pre_integration()
{
    if(std::string("-") != outfile)
        fo = fopen(outfile.c_str(), "w");
}

void MonitorDampingForce::post_step()
{
    double nrm2[2] = { 0.0, 0.0 };
    int    i;

    ++step;
    if(PROC_MD == procel->mytype)
    {
        using namespace md::interface;

        CHECK_PRECOND_ALWAYS(integrator->damper);

        std::valarray<double>& F = integrator->damper->F;

        /// Note: We make use of the fact that only the verlet kick is
        ///       applied after the force modification and hence particles
        ///       have not been moved 
        CHECK_ALWAYS(F.size() == (std::size_t )3*system->NlocalAtoms(), 0);

        system->AccessBegin(System::RD_FORCE);
        for(ParticleHandle ph = system->begin(); ph != system->end(); ++ph)
            for(i = 0; i < DIM; ++i)
            {
                nrm2[0] += F[3*ph.GetLocalId()+i]*F[3*ph.GetLocalId()+i];
                nrm2[1] += Property::Force(*system,ph,i)*Property::Force(*system,ph,i);
            }
        system->AccessEnd();
        
        procel->myscale.allreduce(MPI_IN_PLACE, nrm2, 2, MPI_DOUBLE, MPI_SUM);
        nrm2[0] = sqrt(nrm2[0]);
        nrm2[1] = sqrt(nrm2[1]);

        if(fo)
            fprintf(fo, "%ld\t%e\t%e\n", step, nrm2[0], nrm2[0]/nrm2[1]);
        else
            Log::Instance().write("MonitorDampingForce: %-6ld\t%e\t%e\n", step, nrm2[0], nrm2[0]/nrm2[1]);
    }
    /* Nothing to be done on FE processing elements */
}

void MonitorDampingForce::post_integration()
{
    if(fo)
        fclose(fo);
}

