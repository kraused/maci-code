
// vi: tabstop=4:expandtab

#include <numeric>

#include "FineFluctuationFilter.hh"
#include "FineFluctuationFilterAssembler.hh"
#include "ModifyIndicesAndUpdatePiggybackData.hh"
#include "FineFluctuationFilterJob.hh"
#include "ParticleList.hh"
#include "ParticleListSelector.hh"
#include "Delete.hh"
#include "Logging.hh"
#include "DesignByContract.hh"
#include "MexicoFunctors.hh"


FineFluctuationFilter::FineFluctuationFilter(Maci* app)
: Pointers(app), matrix(0)
{
}

FineFluctuationFilter::~FineFluctuationFilter()
{
    DeletePointer(matrix);
    DeletePointer(cli);
    DeletePointer(job);
}

/// FineFluctuationFilterPiggybackScatterer: Scatter functor used to
/// set up the bdprocs and bddispl
struct FineFluctuationFilterPiggybackScatterer
{
    void operator()(md::interface::PiggybackType* p, int rank, int displacement)
    {
        int i;

        for(i = 0; i < md::interface::PiggybackType::MAX_BDPROCS; ++i)
            if(MPI_PROC_NULL == p->bdprocs[i])
            {
                p->bdprocs[i] = rank;
                p->bddispl[i] = displacement;

                goto fn_exit;
            }

        CHECK_ALWAYS(1 == 0, "Should not get here");
    fn_exit:
        return;
    }
};

void FineFluctuationFilter::CreateMatrix()
{
    int i;

    /// We do not copy the assembled matrix because it
    /// is much too big. Rather we work in-place and
    /// later delete the assembler
    AssemblerOutputMatrix& Q = GetPrimaryAssembler()->matrix;

    ParticleList list;
    if(PROC_MD == procel->mytype)
        list.AddSelection(SelectHandshakeParticles());

    if(PROC_MD == procel->mytype)
        system->AccessBegin(md::interface::System::RD_PIGGYBACK |
                            md::interface::System::WR_PIGGYBACK);
    ModifyIndicesAndUpdatePiggybackData(list, procel->myscale, FineFluctuationFilterPiggybackScatterer(), &Q);
    if(PROC_MD == procel->mytype)
        system->AccessEnd();

    if(PROC_MD == procel->mytype)
    {
        /// The FineFluctuation class depends on the origin
        /// and locIndex members in the PiggybackType to 
        /// compute from where to retrieved the filtered data.
        /// In principle however, these members are independent
        /// of FineFluctuationFilter, so there might be a
        /// better place to set those.
        system->AccessBegin(md::interface::System::RD_PIGGYBACK |
                            md::interface::System::WR_PIGGYBACK);
       
        i = 0;
        for(ParticleList::Iterator it = list.begin(); it != list.end(); ++it)
        {
            md::interface::Property::Piggyback(*system, *it)->origin   = gcpe()->myscale.myrank();
            md::interface::Property::Piggyback(*system, *it)->locIndex = i++;
        }
        system->AccessEnd();

        CHECK_ALWAYS(list.size() == Q.getRelativeNumOfRows(), 0);
        if(list.size() > 0)
        {
            /// We assemble the transfer operator as scalar matrix
            /// but apply it to vectors
            matrix = new linalg::SparseMatrix<double>(3*Q.getRelativeNumOfRows(),
                                                      3*Q.getRelativeNumOfCols());
            matrix->Fill_from_Coo_blk_id_3(Q);
            matrix->Report_stats("FineFluctuationFilter");

            /// Create the job and the mexico instance
            job = new FineFluctuationFilterJob(matrix);
        }
        else
            job = 0;    /// No job to execute

        /// All handshake processing elements are worker
        gather_worker(procel->myscale, (list.size() > 0));
    
        /// Create the mexico instance. The file "FineFluctuationFilter.nml" is 
        /// either located in the current working directory or a default file is
        /// taken from the share folder
        create_cli(procel->myscale, "FineFluctuationFilter.nml");

        if(list.size() > 0)
        {
            /// The fine fluctutation filter maps constant displacements to zero
            /// Here, we check this property and write the residual to the logfile
            std::valarray<double> x(1.0, matrix->NumCols());
            std::valarray<double> y(5.0, matrix->NumRows());    /// Initialized with 5.0 to test
                                                                /// that SpMV works correctly

            matrix->SpMV(linalg::SPMV_OP_NORMAL, 1.0, &x[0], 0.0, &y[0]);
            Log::Instance().write("FineFlucutationFilter: residual = %.3e\n", abs(y).max());
        }
    }

    procel->world.barrier();
    Log::Instance().write("Finished creating the FineFluctuation matrix\n");
}


/// FineFluctuationFilter_Functor: Functor used in FineFluctuationFilter::computeAndStoreInVelocity
struct FineFluctuationFilter_Functor : public MexicoFunctor_i3o3, public MexicoFunctor_2b_fb, public MexicoFunctor_V
{

    void scat(md::interface::System* system, md::interface::ParticleHandle& ph, int k, double* buf) const
    {
        using md::interface::Property;
        CHECK_DEBUG(0 == k, 0);

        Property::Vel(*system, ph, 0) = buf[0];
        Property::Vel(*system, ph, 1) = buf[1];
        Property::Vel(*system, ph, 2) = buf[2];
    }

};


void FineFluctuationFilter::computeAndStoreInVelocity(ParticleList& list, const char* filename)
{
    using namespace md::interface;
    CHECK_PRECOND_ALWAYS(PROC_MD == procel->mytype);

    system->Write(utils::strprintf("%s-pre", filename));
    system->AccessBegin(System::RD_VEL   |
                        System::RD_PIGGYBACK);

    compute(list, FineFluctuationFilter_Functor());

    system->AccessEnd();
    system->Write(utils::strprintf("%s-post", filename));
}

