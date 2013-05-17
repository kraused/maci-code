
// vi: tabstop=4:expandtab

#include "RattleCorrector.hh"
#include "Maci.hh"
#include "TauProfiling.hh"
#include "NodalValuePacker.hh"
#include "NodalValueScatterer.hh"
#include "Delete.hh"
#include "NodalValueBuffer.hh"
#include "HandshakeGeo.hh"
#include "MeshMassMatrix.hh"
#include "TransferOperator.hh"
#include "MultiplierMatrix.hh"
#include "RattleCorrectorHelper.hh"
#include "Weights.hh"
#include "MexicoFunctors.hh"
#include "RattleCorrectorJob.hh"


RattleCorrector::RattleCorrector(Maci* app, double tau, TransferOperator* R, MeshMassMatrix* constraintsM, MeshMassMatrix* correctionM, MultiplierMatrix* L)
: DAEqCorrector(app), tau(tau), R(R), constraintsM(constraintsM), correctionM(correctionM), L(L)
{
    if(PROC_FE == procel->mytype)
    {
        NodeList list(*handshake, SelectOwnedNodes());

        u = new NodalValueBuffer<double>(list);
        g = new NodalValueBuffer<double>(list);
    
        if(R->IsNonNull())
        {
            job = new RattleCorrectorJob(R, constraintsM, u, g, L);
            /// The member nu is set in CorrectAfterDickDrift() to lambda an 
            /// in CorrectAfterKick() to mu
        }
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
    gather_worker(procel->world, (PROC_FE == procel->mytype) and R->IsNonNull());

    /// Create the mexico instance. The file "RattleCorrector.nml" is 
    /// either located in the current working directory or a default file is
    /// taken from the share folder
    create_cli(procel->world, "RattleCorrector.nml");
}

RattleCorrector::~RattleCorrector()
{
    DeletePointer(u);
    DeletePointer(g);
    DeletePointer(cli);
    DeletePointer(job);
}

void RattleCorrector::CorrectAfterKickDrift(NodalValueBuffer<double>* lambda)
{
    if(PROC_FE == procel->mytype)
    {
        CorrectAfterKickDrift(NULL, lambda);
    }
    if(PROC_MD == procel->mytype)
    {
        SelectHandshakeParticles selector;
        ParticleList list(selector);

        CorrectAfterKickDrift(&list, lambda);
    }
}


struct RattleCorrector_RattleCorrector_FunctorKickDrift : public MexicoFunctor_i3o3, public MexicoFunctor_2f_ff, public MexicoFunctor_U
{

    RattleCorrector_RattleCorrector_FunctorKickDrift(double tau)
    : tau(tau)
    {
    }

    void scat(md::interface::System* system, md::interface::ParticleHandle& ph, int k, double* buf) const
    {
        using md::interface::Property;

        double bx, by, bz;

        bx = buf[0]/Property::WeightedMass(*system, ph);
        by = buf[1]/Property::WeightedMass(*system, ph);
        bz = buf[2]/Property::WeightedMass(*system, ph);

        Property::Pos(*system, ph, 0) += bx;
        Property::Pos(*system, ph, 1) += by;
        Property::Pos(*system, ph, 2) += bz;

        Property::Vel(*system, ph, 0) += bx/tau;
        Property::Vel(*system, ph, 1) += by/tau;
        Property::Vel(*system, ph, 2) += bz/tau;
    }

    /// The timestep size
    double tau;

};


void RattleCorrector::CorrectAfterKickDrift(ParticleList* list, NodalValueBuffer<double>* lambda)
{
    double t0, t1;

    TAU_PROFILE("RattleCorrector::CorrectAfterKickDrift", "", TAU_DEFAULT);

    t0 = MPI_Wtime();

    if(PROC_MD == procel->mytype)
    {
        system->AccessBegin(md::interface::System::RD_POS  |
                            md::interface::System::WR_POS  |
                            md::interface::System::RD_REFP |
                            md::interface::System::WR_REFP |
                            md::interface::System::RD_VEL  |
                            md::interface::System::WR_VEL  |
                            md::interface::System::RD_PIGGYBACK);

        RattleCorrector_RattleCorrector_FunctorKickDrift f(tau);

        pack(*list, f);
        execute_cli();
        scat(*list, f);
        
        system->AccessEnd();
    }
    else
    {
        solver->AccessBegin(fe::interface::Solver::RD_DISP |
                            fe::interface::Solver::WR_DISP |
                            fe::interface::Solver::RD_VEL  |
                            fe::interface::Solver::WR_VEL);

        if(job) /// NULL == job if the local subdomain does not
                /// intersect the handshake region
            ((RattleCorrectorJob* )job)->nu = lambda;

        u->forall(NodalValuePackDispl());
        execute_cli();
        
        /// Reuse u to compute the correction
        correctionM->SpMV(linalg::SPMV_OP_NORMAL, 1.0, &(*lambda)[0], 0.0, &(*u)[0]);
        u->forall(RattleNodalValueCorrectorDisplVel(tau));

        solver->AccessEnd();
    }

    t1 = MPI_Wtime();
    time_spend += t1 - t0;  /// Sum them up: The ShowProgress plugin
                            /// is computing the average
}


struct RattleCorrector_FunctorKick : public MexicoFunctor_i3o3, public MexicoFunctor_2f_ff, public MexicoFunctor_V
{

    void scat(md::interface::System* system, md::interface::ParticleHandle& ph, int k, double* buf) const
    {
        using md::interface::Property;

        double bx, by, bz;

        bx = buf[0]/Property::WeightedMass(*system, ph);
        by = buf[1]/Property::WeightedMass(*system, ph);
        bz = buf[2]/Property::WeightedMass(*system, ph);

        Property::Vel(*system, ph, 0) += bx;
        Property::Vel(*system, ph, 1) += by;
        Property::Vel(*system, ph, 2) += bz;
    }

};


void RattleCorrector::CorrectAfterKick(NodalValueBuffer<double>* mu)
{
    if(PROC_FE == procel->mytype)
    {
        CorrectAfterKick(NULL, mu);
    }
    if(PROC_MD == procel->mytype)
    {
        SelectHandshakeParticles selector;
        ParticleList list(selector);

        CorrectAfterKick(&list, mu);
    }
}

void RattleCorrector::CorrectAfterKick(ParticleList* list, NodalValueBuffer<double>* mu)
{
    double t0, t1;

    TAU_PROFILE("RattleCorrector::CorrectAfterKick", "", TAU_DEFAULT);

    t0 = MPI_Wtime();

    if(PROC_MD == procel->mytype)
    {
        system->AccessBegin(md::interface::System::RD_VEL  |
                            md::interface::System::WR_VEL  |
                            md::interface::System::RD_PIGGYBACK);

        RattleCorrector_FunctorKick f;

        pack(*list, f);
        execute_cli();
        scat(*list, f);

        system->AccessEnd();
    }
    else
    {
        solver->AccessBegin(fe::interface::Solver::RD_VEL  |
                            fe::interface::Solver::WR_VEL);

        if(job) /// NULL == job if the local subdomain does not
                /// intersect the handshake region
            ((RattleCorrectorJob* )job)->nu = mu;

        u->forall(NodalValuePackVel());
        execute_cli();

        /// Reuse u to compute the correction
        correctionM->SpMV(linalg::SPMV_OP_NORMAL, 1.0, &(*mu)[0], 0.0, &(*u)[0]);
        u->forall(RattleNodalValueCorrectorVel(tau));

        solver->AccessEnd();
    }

    t1 = MPI_Wtime();
    time_spend += t1 - t0;  /// Sum them up: The ShowProgress plugin
                            /// is computing the average
}

ResidualNormComputer* RattleCorrector::NewResidualNormComputer()
{
    return new ResidualNormComputer(app, R, constraintsM);
}

