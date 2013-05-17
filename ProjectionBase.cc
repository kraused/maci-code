
// vi: tabstop=4:expandtab

#include "ProjectionBase.hh"
#include "NodalValueBuffer.hh"
#include "NodalValueScatterer.hh"
#include "MexicoFunctors.hh"
#include "ProjectionBaseJob.hh"

#include "utils/strprintf.hh"


ProjectionBase::ProjectionBase(Maci* app)
: TransferOperator(app)
{
    if(PROC_FE == procel->mytype)
    {
        NodeList list(*handshake, SelectOwnedNodes());

        u = new NodalValueBuffer<double>(list);

        if(IsNonNull())
            job = new ProjectionBaseJob(this, u);
        else
            job = 0;

        /// We need to set i_cnt and o_cnt by hand because
        /// we never call scat() or gat()
        i_cnt = 3;
        o_cnt = 3;
    }
    if(PROC_MD == procel->mytype)
    {
        u = 0;

        job = 0;
    }

    /// All handshake FE processing elements are the worker
    gather_worker(procel->world, (PROC_FE == procel->mytype) and IsNonNull() );

    /// Create the mexico instance. The file "ProjectionBase.nml" is 
    /// either located in the current working directory or a default file is
    /// taken from the share folder
    create_cli(procel->world, "ProjectionBase.nml");
}


/// FunctorV: Gather velocities. Since we never call scat() we don't need to
///           implement a FunctorV::scat() function
class ProjectionBase_FunctorV : public MexicoFunctor_i3o3, public MexicoFunctor_2f, public MexicoFunctor_fn, public MexicoFunctor_V
{
};


void ProjectionBase::project(ParticleList *list, const char* filename)
{
    if(PROC_MD == procel->mytype)
    {
        ProjectionBase_FunctorV f;
 
        CHECK_PRECOND_ALWAYS(list);

        system->AccessBegin(md::interface::System::RD_VEL  |
                            md::interface::System::RD_PIGGYBACK);
        pack(*list, f);
        system->AccessEnd();

        execute_cli();

        solver->Write(utils::strprintf("%s-md", filename));
    }
    else
    {
        execute_cli();

        solver->AccessBegin(fe::interface::Solver::WR_VEL);
        u->forall(NodalValueScatterVel());
        solver->AccessEnd();

        solver->Write(utils::strprintf("%s-fe", filename));
    }
}

