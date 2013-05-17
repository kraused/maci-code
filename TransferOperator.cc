
// vi: tabstop=4:expandtab

#include "TransferOperator.hh"
#include "Delete.hh"
#include "Logging.hh"
#include "ModifyIndicesAndUpdatePiggybackData.hh"


TransferOperator::TransferOperator(Maci* app)
: Pointers(app), A(0)
{
}


TransferOperator::~TransferOperator()
{
    DeletePointer(A);
}

bool TransferOperator::IsNonNull() const
{
    return (0 != A);
}

int TransferOperator::NumRows() const
{
    if(A)
        return A->NumRows();
    else
        return 0;
}

int TransferOperator::NumCols() const
{
    if(A)
        return A->NumCols();
    else
        return 0;
}

void TransferOperator::SpMV(linalg::SpMV_Op op, double alpha, double* x, double beta, double* b)
{
    if(A)
        A->SpMV(op, alpha, x, beta, b);
    /// else: do nothing
}

/// TransferOperatorPiggybackScatterer: Scatter functor used to
/// set up the feprocs and fedispl
struct TransferOperatorPiggybackScatterer
{
    void operator()(md::interface::PiggybackType* p, int rank, int displacement)
    {
        int i;

        for(i = 0; i < md::interface::PiggybackType::MAX_FEPROCS; ++i)
            if(MPI_PROC_NULL == p->feprocs[i])
            {
                p->feprocs[i] = rank;
                p->fedispl[i] = displacement;

                goto fn_exit;
            }

        CHECK_ALWAYS(1 == 0, "Should not get here");
    fn_exit:
        return;
    }
};

void TransferOperator::CreateMatrix()
{
    /// Copy the output matrix so that we can 
    /// modify the indices
    AssemblerOutputMatrix R = GetPrimaryAssembler()->matrix;

    ParticleList list;
    if(PROC_MD == procel->mytype)
        list.AddSelection(SelectHandshakeParticles());

    if(PROC_MD == procel->mytype)
        system->AccessBegin(md::interface::System::RD_PIGGYBACK |
                            md::interface::System::WR_PIGGYBACK);
    ModifyIndicesAndUpdatePiggybackData(list, procel->world, TransferOperatorPiggybackScatterer(), &R);
    if(PROC_MD == procel->mytype)
        system->AccessEnd();

    if(PROC_FE == procel->mytype)
    {
        if(0 == R.getTotalNumOfRows() || 0 == R.getTotalNumOfCols())
            goto fn_exit;

        /// We assemble the transfer operator as scalar matrix
        /// but apply it to vectors
        A = new linalg::SparseMatrix<double>(3*R.getTotalNumOfRows(), 3*R.getTotalNumOfCols());
        A->Fill_from_Coo_blk_id_3(R);
        A->Report_stats("TransferOperator");
    }

fn_exit:
    procel->world.barrier();
    Log::Instance().write("Finished TransferOperator::CreateMatrix()\n");
}

