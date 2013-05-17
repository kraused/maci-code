
// vi: tabstop=4:expandtab

#include "linalg/CoordMatrix.hh"
#include "Int64.hh"
#include "DesignByContract.hh"
#include "Logging.hh"

#include <cstdio>
#include <valarray>

namespace linalg 
{

    template<typename Ti, typename Tp>
    static void constructDatatype(MPI_Datatype *type);

    template<>
    void constructDatatype<int64,double>(MPI_Datatype *type)
    {
        typedef CoordMatrixEntry<int64,double> Entry;
        create_struct3(*type, Entry, val, MPI_DOUBLE       , 1,
                                 row, MPI_LONG_LONG_INT, 1,
                                 col, MPI_LONG_LONG_INT, 1);
        MPI_Type_commit(type);

        CHECK_POSTCOND_ALWAYS(MPI_DATATYPE_NULL != (*type));
    }

    template<>
    MPI_Datatype CoordMatrixEntry<int64,double>::type()
    {
        static MPI_Datatype t = MPI_DATATYPE_NULL;

        if(MPI_DATATYPE_NULL == t) {
            constructDatatype<int64,double>(&t);
        }

        return t;   
    }

    /// Fill the row information, i.e. the row index vector, the row pointers
    /// and the row lengths
    void FillRowInfo(const CoordMatrix<int64,double>& A, std::size_t n,
                     std::valarray<int64> *idx,
                     std::valarray<int>   *ptr,
                     std::valarray<int>   *length)
    {
        if(0 == n)
            return;

        std::size_t k = 0;

           (*idx)[k] = A[0].row;
        (*length)[k] = 1;
           (*ptr)[k] = 0;

        for(std::size_t i = 1; i < A.size(); ++i) 
        {
            CHECK_DEBUG(k < n, 0);

            if(A[i].row == (*idx)[k]) {
                ++(*length)[k];
            } else {
                ++k;
                   (*idx)[k] = A[i].row;
                (*length)[k] = 1;
                   (*ptr)[k] = i;
            }
        }
    }

    /// Fill the row information, i.e. the row index vector, the row pointers
    /// and the row lengths
    void FillColInfo(const CoordMatrix<int64,double>& B, std::size_t n,
                     std::valarray<int64> *idx,
                     std::valarray<int>   *ptr,
                     std::valarray<int>   *length)
    {
        if(0 == n)
            return;

        std::size_t k = 0;

           (*idx)[k] = B[0].col;
        (*length)[k] = 1;
           (*ptr)[k] = 0;

        for(std::size_t i = 1; i < B.size(); ++i) {
            CHECK_DEBUG(k < n, 0);

            if(B[i].col == (*idx)[k]) {
                ++(*length)[k];
            } else {
                ++k;
                   (*idx)[k] = B[i].col;
                (*length)[k] = 1;
                   (*ptr)[k] = i;
            }
        }
    }

    /// Compute the dot product of two sparse vectors
    /// Both vectors must be formated
    double SparseDot(const CoordMatrixEntry<int64,double> *a, std::size_t n,
                     const CoordMatrixEntry<int64,double> *b, std::size_t m)
    {
        double      s    = 0.0;
        std::size_t i, j = 0;

        if(0 == n || 0 == m)
            return 0.0;

        if(a[0].col >= b[0].row) 
        {
            for(i = 0; i < n; ++i) 
            {
                while(j < m && b[j].row < a[i].col)
                    ++j;
                if(j == m)
                    break;
                if(b[j].row != a[i].col)
                    continue;
    
                CHECK_DEBUG(i < n, 0);
                CHECK_DEBUG(j < m, 0);
                CHECK_DEBUG(b[j].row == a[i].col, 0);
    
                s += a[i].val*b[j++].val;
            }
        } 
        else 
        {
            for(i = 0; i < m; ++i) 
            {
                while(j < n && a[j].col < b[i].row)
                    ++j;
                if(j == n)
                    break;
                if(a[j].col != b[i].row)
                    continue;
    
                CHECK_DEBUG(j < n, 0);
                CHECK_DEBUG(i < m, 0);
                CHECK_DEBUG(a[j].col == b[i].row, 0);
    
                s += a[j++].val*b[i].val;
            }
        }

        return s;
    }

    template<>
    void CoordMatrix<int64,double>::coomm(double alpha, CoordMatrix<int64,double>& A, CoordMatrix<int64,double>& B, double beta , CoordMatrix<int64,double>* C)
    {
        std::size_t n, m, i, j;
        double c;    

        n = A.countNumNzRows();
        m = B.countNumNzCols();

        std::valarray<int64> rowidx(n), colidx(m);
        std::valarray<int>   rowptr(n), rowlength(n), colptr(m), collength(m);

        if(0.0 == beta)
            C->clear(); /// Question: Is it better to clear also the memory at this point?
        else
        if(1.0 == beta)
            ;           /// Do nothing
        else
            C->ScaleVals(beta);

        if(0 == n || 0 == m)
            goto fn_exit;

        A.sort();

        /// In order to sort rows by column indices we use a trick:
        /// We swap the values (transpose the matrix), then sort by
        /// row and then swap back
        B.Transpose();
        B.sort();
        B.Transpose();

        FillRowInfo(A, n, &rowidx, &rowptr, &rowlength);
        FillColInfo(B, m, &colidx, &colptr, &collength);

        for(i = 0; i < n; ++i) 
        {
            for(j = 0; j < m; ++j) 
            {
                CHECK_DEBUG(rowptr[i] >= 0 && rowlength[i] >= 0 && rowptr[i] + rowlength[i] <= (int64 )A.size(), 0);
                CHECK_DEBUG(colptr[j] >= 0 && collength[j] >= 0 && colptr[j] + collength[j] <= (int64 )B.size(), 0);

                c = SparseDot(&A[rowptr[i]], rowlength[i], &B[colptr[j]], collength[j]);
                if(fabs(c) > 1e-16)
                    C->insert(rowidx[i], colidx[j], alpha*c);
            }

            if(0 == i % 100) 
                C->SumupDuplicates();
        }
        C->SumupDuplicates();

    fn_exit:
        ;
    }

    template<>
    void CoordMatrix<int64,double>::write(const char *filename) const
    {
        FILE* fh;

        CHECK_PRECOND_ALWAYS(filename);

        fh = fopen(filename, "w");
        CHECK_ALWAYS(fh, "error opening file");

        fprintf(fh, "%s matrix coordinate real general\n", "%%MatrixMarket");
        fprintf(fh, "%lu %lu %lu\n", getTotalNumOfRows(), getTotalNumOfCols(), size());
        for(ConstIterator a = begin(); a != end(); ++a)
            fprintf(fh, "%ld %ld %f\n", (long )a->row, (long )a->col, a->val);

        fclose(fh);
    }

}

