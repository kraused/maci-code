
// vi: tabstop=4:expandtab

#include "CSRHelper.hh"
#include "Int64.hh"
#include "DesignByContract.hh"

#include "linalg/CoordMatrix.hh"

template<typename Tp, typename Ti, typename Tq>
void linalg::Coo_blk_id_3_to_CSR(const CoordMatrix<Ti, Tq>& A, int nrows, int ncols, int* nnz, int** ap, int** ai, Tp** av)
{
    (*nnz) = 3*A.size();        // Number of nonzeroes in the matrix

    (*ap) = new int[nrows+1]();
    (*ai) = new int[(*nnz)];
    (*av) = new Tp [(*nnz)];

    for(size_t i = 0; i < A.size(); ++i)
    {
        CHECK_DEBUG((int )3*A[i].row+2 >= 0    , 0);
        CHECK_DEBUG((int )3*A[i].row+2  < nrows, 0);

        (*ap)[3*A[i].row+0] += 1;
        (*ap)[3*A[i].row+1] += 1;
        (*ap)[3*A[i].row+2] += 1;
    }

    for(int i = nrows; i > 0; --i)
        (*ap)[i] = (*ap)[i-1];
    (*ap)[0] = 0;
    for(int i = 1; i <= nrows; ++i)
        (*ap)[i] += (*ap)[i-1];

    CHECK_DEBUG((*ap)[nrows] == (int )(*nnz), "Error in the calculation of the row pointers.");

    std::fill((*ai), (*ai)+(*nnz), -1);

    for(size_t i = 0; i < A.size(); ++i)
        for(int j = 0; j < 3; ++j)
        {
            CHECK_DEBUG((int )3*A[i].row+j >= 0    , 0);
            CHECK_DEBUG((int )3*A[i].row+j  < nrows, 0);

            bool bf = false;

            for(int k = (*ap)[3*A[i].row+j]; k < (*ap)[3*A[i].row+j+1]; ++k)
            {
                CHECK_DEBUG(k >= 0 && k < (int )(*nnz), 0);

                if(-1 == (*ai)[k])
                {
                    (*ai)[k] = 3*A[i].col+j;
                    (*av)[k] = (j <= DIM) ? (Tp )A[i].val : 0.0;

                    bf = true;
                    break;
                }
            }

            CHECK_ALWAYS(bf, "No free slot found. Should not happen.");
        }

#ifndef NDEBUG
    for(long i = 0; i < (*nnz); ++i)
        CHECK_DEBUG((*ai)[i] >= 0 && (*ai)[i] < ncols, 0);
#endif

}

template
void linalg::Coo_blk_id_3_to_CSR<double, int64, double>(const CoordMatrix<int64, double>&, int, int, int*, int**, int**, double**);
template
void linalg::Coo_blk_id_3_to_CSR<float , int64, double>(const CoordMatrix<int64, double>&, int, int, int*, int**, int**, float **);

