
// vi: tabstop=4:expandtab

#include "ResidualNormComputer.hh"
#include "NodalValuePacker.hh"
#include "NodeList.hh"
#include "Maci.hh"
#include "Delete.hh"
#include "ResidualNormComputerJob.hh"
#include "TauProfiling.hh"
#include "MexicoFunctors.hh"


ResidualNormComputer::ResidualNormComputer(Maci* app, TransferOperator* R, MeshMassMatrix* M)
: Pointers(app)
{

    /// We do not store the pointers to R and M but pass
    /// them directly to the job

    if(PROC_FE == procel->mytype)
    {
        NodeList list(*app->handshake, SelectOwnedNodes());

        u = new NodalValueBuffer<double>(list);
        g = new NodalValueBuffer<double>(list);

        if(R->IsNonNull())
            job = new ResidualNormComputerJob(R, M, u, g);
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
        g = 0;

        job = 0;
    }

    /// All handshake FE processing elements are the worker
    gather_worker(procel->world, (PROC_FE == procel->mytype) and R->IsNonNull() );

    /// Create the mexico instance. The file "ResidualNormComputer.nml" is 
    /// either located in the current working directory or a default file is
    /// taken from the share folder
    create_cli(procel->world, "ResidualNormComputer.nml");
}

ResidualNormComputer::~ResidualNormComputer()
{
    DeletePointer(u);
    DeletePointer(g);
    DeletePointer(cli);
    DeletePointer(job);
}


/// ResidualNormComputer_FunctorU: Gather displacements. Since we never call scat() we don't need to
///                                implement a FunctorU::scat() function
class ResidualNormComputer_FunctorU : public MexicoFunctor_i3o3, public MexicoFunctor_2f, public MexicoFunctor_fn, public MexicoFunctor_U
{
};


double ResidualNormComputer::Nrm2U()
{
    double nrm2 = 0;

    TAU_PROFILE("ResidualNormComputer::Nrm2U", "", TAU_DEFAULT);

    if(PROC_MD == procel->mytype)
    {
        ResidualNormComputer_FunctorU f;
        SelectHandshakeParticles selector;
        ParticleList list(selector);

        system->AccessBegin(md::interface::System::RD_POS  |
                            md::interface::System::RD_REFP |
                            md::interface::System::RD_PIGGYBACK);
        pack(list, f);
        system->AccessEnd();

        execute_cli();
    }
    else
    {
        solver->AccessBegin(fe::interface::Solver::RD_DISP);
        u->forall(NodalValuePackDispl());
        solver->AccessEnd();

        execute_cli();

        nrm2 = g->norml2();
    }

    procel->world.allreduce(MPI_IN_PLACE, &nrm2, 1, MPI_DOUBLE, MPI_SUM);
    return nrm2;
}


/// ResidualNormComputer_FunctorV: Gather velocities. Since we never call scat() we don't need to
///                                implement a FunctorV::scat() function
class ResidualNormComputer_FunctorV : public MexicoFunctor_i3o3, public MexicoFunctor_2f, public MexicoFunctor_fn, public MexicoFunctor_V
{
};


double ResidualNormComputer::Nrm2V()
{
    double nrm2 = 0;

    TAU_PROFILE("ResidualNormComputer::Nrm2V", "", TAU_DEFAULT);

    if(PROC_MD == procel->mytype)
    {
        ResidualNormComputer_FunctorV f;
        SelectHandshakeParticles selector;
        ParticleList list(selector);

        system->AccessBegin(md::interface::System::RD_VEL  |
                            md::interface::System::RD_PIGGYBACK);
        pack(list, f);
        system->AccessEnd();

        execute_cli();
    }
    else
    {
        solver->AccessBegin(fe::interface::Solver::RD_VEL);
        u->forall(NodalValuePackVel());
        solver->AccessEnd();
        
        execute_cli();

        nrm2 = g->norml2();
    }

    procel->world.allreduce(MPI_IN_PLACE, &nrm2, 1, MPI_DOUBLE, MPI_SUM);
    return nrm2;
}

