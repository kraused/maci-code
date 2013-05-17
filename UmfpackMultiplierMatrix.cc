
// vi: tabstop=4:expandtab

#include "UmfpackMultiplierMatrix.hh"
#include "linalg/SerialUmfpackSolver.hh"


UmfpackMultiplierMatrix::UmfpackMultiplierMatrix(Maci* app)
: MultiplierMatrix(app)
{

}

void UmfpackMultiplierMatrix::modifyIndices(MultiplierAssembler *ass)
{
    const mpi::Communicator& graphcomm = ass->commGraph().comm();

    if(0 == graphcomm.size()) {
        CHECK_PRECOND_ALWAYS(0 == ass->matrix().size());
        return;
    }
    CHECK_PRECOND_ALWAYS(graphcomm.size() > 0);

    graphcomm.barrier();

    Assembler::OutputMatrixType& mat = ass->matrix();

    int cnt = 0;

    for(std::size_t i = 0; i < mat.size(); ++i) {
        CHECK_DEBUG(graphcomm.myrank() == Int64::hi(mat[i].row), 0);
        
        if(graphcomm.myrank() == Int64::hi(mat[i].col)) {
            mat[i].row = (int64 )Int64::lo(mat[i].row);
            mat[i].col = (int64 )Int64::lo(mat[i].col);
        } else {
            ++cnt;
        }
    }
    mat.removeDuplicates();

    if(cnt > 0) {
        printf(" [%d]: %s:%d: WARNING: removed %d entries\n",
            graphcomm.myrank(), __FILE__,__LINE__, cnt);
    }
}

void UmfpackMultiplierMatrix::createSolver(MultiplierAssembler *ass)
{
    modifyIndices(ass);
    this->solver = new linalg::SerialUmfpackSolver(ass);
}

