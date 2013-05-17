
// vi: tabstop=4:expandtab

#ifndef MACI_LINALD_COORD_MATRIX_HH_INCLUDED
#define MACI_LINALD_COORD_MATRIX_HH_INCLUDED 1

#include "PlainOldData.hh"
#include "linalg/CoordMatrixDuplicatesHelper.hh"

#include "mpi/Datatype.hh"

#include <cmath>
#include <vector>

namespace linalg 
{

    /// A matrix entry
    /// We use an array of triplets instead of three single arrays
    /// because this enables us to use the STL sorting functions
    /// in a more straight forward fashion
    template<typename Ti, typename Tp>
    struct CoordMatrixEntry : public PlainOldData<CoordMatrixEntry<Ti, Tp>/**/>
    {
        Tp val;
        Ti row;
        Ti col;

        /// Default constructor
        CoordMatrixEntry()
        {
        }

        /// Create a new instance
        /// @param r row index
        /// @param c column index
        /// @param v value
        CoordMatrixEntry(Ti r, Ti c, Tp v)
        : val(v), row(r), col(c)
        {
        }

        /// Copy constructor
        CoordMatrixEntry(const CoordMatrixEntry& e)
        : val(e.val), row(e.row), col(e.col)
        {
        }

        /// Compare two entries
        /// @param e    the other entry
        /// @returns    true if the row index of this is smaller
        ///     of the row index of e or if both row
        ///     indices coincide and the column index
        ///     of this is smaller than the column index
        ///     of e
        inline bool operator<(const CoordMatrixEntry& e) const
        {
            return (row < e.row) || 
                  ((row == e.row) && (col < e.col));
        }

        /// Compare two entries based on the indices
        /// @param e    the other entry
        /// @returns    true if the row indices or the column
        ///     indices do match
        inline bool operator==(const CoordMatrixEntry& e) const
        {
            return (row == e.row) && (col == e.col);
        }

        /// Compare two entries based on the indices
        /// @param e    the other entry
        /// @returns    true if the row indices or the column
        ///     indices do not match
        inline bool operator!=(const CoordMatrixEntry& e) const
        {
            return (row != e.row) || (col != e.col);
        }

        /// Sum up entries
        /// @param e    the other entry
        inline CoordMatrixEntry& operator+=(const CoordMatrixEntry& e)
        {
            val += e.val;
            return (*this);
        }

        /// Assignment operator
        /// @param e    the other entry
        inline CoordMatrixEntry& operator=(const CoordMatrixEntry& e)
        {
            val = e.val;
            row = e.row;
            col = e.col;

            return (*this);
        }

        /// Get the MPI datatype for the data exchange
        /// @remark not thread safe; Moreover the commited datatype is not 
        ///     freed before MPI is finalized
        /// @returns    Handle for a committed datatype
        static MPI_Datatype type();

    };

    /// A coordinate sparse matrix
    /// Since the actual size of the matrix doesn't matter for the coordinate
    /// format we don't ask for it. This makes the class much more flexible
    /// e.g. because row and column indices don't have to be contiguous
    template<typename Ti, typename Tp>
    class CoordMatrix : public std::vector<CoordMatrixEntry<Ti, Tp>/**/>
    {
    public:
        typedef CoordMatrixEntry<Ti, Tp>    Entry;
    private:
        typedef std::vector<Entry>          Base;
        typedef typename Base::size_type    SizeType;
    public:
        typedef typename Base::iterator         Iterator;
        typedef typename Base::const_iterator   ConstIterator;

        /// Create a new empty coordinate matrix
        CoordMatrix()
        : Base()
        {
        }

        /// Copy constructor: creates a deep copy
        /// @param mat  the other matrix
        CoordMatrix(const CoordMatrix& mat)
        : Base(mat)
        {
        }
        
        /// Insert a new entry
        /// @param row the row index
        /// @param col the column index
        /// @param val value
        void insert(Ti row, Ti col, Tp val)
        {
            Base::push_back(Entry(row,col,val));
        }

        /// Insert an entry
        /// @param e    the entry to insert
        void insert(const Entry& e)
        {
            Base::push_back(e);
        }

        /// Remove duplicate entries
        void removeDuplicates()
        {
            typename std::vector<Entry>::iterator i = CoordMatrixDuplicatesHelper::remove(Base::begin(), Base::end());
            Base::erase(i,Base::end());
        }

        /// Sum up duplicate entries
        void SumupDuplicates()
        {
#ifndef NDEBUG
            Tp sum1 = Tp();
            for(SizeType k = 0; k < Base::size(); ++k)
                sum1 += (*this)[k].val;
#endif
            typename std::vector<Entry>::iterator i = CoordMatrixDuplicatesHelper::sumup(Base::begin(), Base::end());
            Base::erase(i,Base::end());

#ifndef NDEBUG
            Tp sum2 = Tp();
            for(SizeType k = 0; k < Base::size(); ++k)
                sum2 += (*this)[k].val;
        
            Tp diff = sum1 - sum2;
            Tp max  = std::max(fabs(sum1), fabs(sum2));

            CHECK_POSTCOND_DEBUG(fabs(diff) <= std::max(1e-9, 1e-5*max));
#endif
        }

        /// Get the minimal row index
        std::size_t getMinRowIdx() const
        {
            if(0 == this->size()) {
                return 0;
            }

            Ti n = (*this)[0].row;
            for(SizeType i = 1; i < Base::size(); ++i) {
                n = std::min(n, (*this)[i].row);
            }

            CHECK_POSTCOND_DEBUG(n >= 0);
            return (std::size_t )n;
        }

        /// Get the number of rows in the matrix (relative version)
        std::size_t getRelativeNumOfRows() const
        {
            if(0 == this->size())
                return 0;

            Ti n = (*this)[0].row, 
               k = (*this)[0].row;
            for(SizeType i = 1; i < Base::size(); ++i) 
            {
                n = std::max(n, (*this)[i].row);
                k = std::min(k, (*this)[i].row);
            }
            // Indices start at 0
            n = n+1 - k;

            CHECK_POSTCOND_DEBUG(0 == Base::size() || n >= 1);
            return (std::size_t )n;
        }

        /// Get the number of rows in the matrix (total version)
        std::size_t getTotalNumOfRows() const
        {
            if(0 == this->size())
                return 0;

            Ti n = (*this)[0].row;
            for(SizeType i = 1; i < Base::size(); ++i)
                n = std::max(n, (*this)[i].row);
            
            // Indices start at 0
            n = n+1;

            CHECK_POSTCOND_DEBUG(0 == Base::size() || n >= 1);
            return (std::size_t )n;
        }

        /// Get the minimal column index
        std::size_t getMinColIdx() const
        {
            if(0 == this->size())
                return 0;

            Ti n = (*this)[0].col;
            for(SizeType i = 1; i < Base::size(); ++i)
                n = std::min(n, (*this)[i].col);

            CHECK_POSTCOND_DEBUG(n >= 0);
            return (std::size_t )n;
        }

        /// Get the number of columns in the matrix (relative version)
        std::size_t getRelativeNumOfCols() const
        {
            if(0 == this->size())
                return 0;

            Ti n = (*this)[0].col, 
               k = (*this)[0].col;
            for(SizeType i = 0; i < Base::size(); ++i) 
            {
                n = std::max(n, (*this)[i].col);
                k = std::min(k, (*this)[i].col);
            }
            // Indices start at 0
            n = n+1 - k;

            CHECK_POSTCOND_DEBUG(0 == Base::size() || n >= 1);
            return (std::size_t )n;
        }

        /// Get the number of columns in the matrix (total version)
        std::size_t getTotalNumOfCols() const
        {
            if(0 == this->size())
                return 0;

            Ti n = (*this)[0].col;
            for(SizeType i = 0; i < Base::size(); ++i) 
            {
                n = std::max(n, (*this)[i].col);
            }
            // Indices start at 0
            n = n+1;

            CHECK_POSTCOND_DEBUG(0 == Base::size() || n >= 1);
            return (std::size_t )n;
        }

        /// Count number of nonzero rows
        /// @warning Assumes sorted rows
        std::size_t countNumNzRows() const
        {
            if(0 == this->size())
                return 0;

            std::size_t n = 1;
            Ti p = (*this)[0].row;
            for(std::size_t i = 1; i < this->size(); ++i) 
            {
                if(p != (*this)[i].row) 
                {
                    ++n;
                    p = (*this)[i].row;
                }
            }

            return n;
        }

        /// Count number of nonzero columns
        /// @warning Assumes sorted columns
        std::size_t countNumNzCols() const
        {
            if(0 == this->size())
                return 0;

            std::size_t n = 1;
            Ti p = (*this)[0].col;
            for(std::size_t i = 1; i < this->size(); ++i) 
            {
                if(p != (*this)[i].col) 
                {
                    ++n;
                    p = (*this)[i].col;
                }
            }

            return n;
        }

        /// Sort entries by rows
        void sort()
        {
            std::sort(Base::begin(), Base::end());
        }

        /// Transpose the matrix
        void Transpose()
        {
            for(Iterator e = this->begin(); e != this->end(); ++e) 
                std::swap(e->col, e->row);
        }

        /// Invert the entries in the matrix. Note that the function
        /// doesn't handle zero entries
        void InvertVals()
        {
            for(Iterator e = this->begin(); e != this->end(); ++e)
                e->val = 1.0/e->val;
        }

        /// Scale all the entries of the matrix
        void ScaleVals(Tp alpha)
        {
            for(Iterator e = this->begin(); e != this->end(); ++e)
                e->val *= alpha;
        }

        /// Return the entry datatype
        inline MPI_Datatype type() const
        {
            return Entry::type();
        }

        /// Compute matrix matrix product
        /// <code>
        /// C = beta*C + alpha*A*B
        /// </code>
        /// @param alpha    scalar
        /// @param A        first matrix
        /// @param B        second matrix
        /// @param beta     second scalar
        /// @param C        output matrix
        ///
        /// @warning The function might reorder the matrices A and B
        ///      to speedup the execution
        static void coomm(Tp alpha, CoordMatrix<Ti, Tp>& A, CoordMatrix<Ti, Tp>& B, Tp beta, CoordMatrix<Ti, Tp>* C);

        /// Compute the maximal row size
        std::size_t maxBandwidth() const
        {
            if(0 == this->size())
                return 0;

            Ti p = (*this)[0].row;

            std::size_t n = 0, m = 0;
            
            for(ConstIterator a = this->begin(); a != this->end(); ++a) 
            {
                if(p == a->row)
                {
                    ++n;
                }
                else 
                {
                    m = std::max(m,n);
                    n = 1;
                    p = a->row;
                }
            }

            return std::max(m,n);
        }

        /// Release the matrix memory
        void FreeMemory()
        {
            CoordMatrix<Ti, Tp>().swap(*this);
        }

        /// Write matrix to a MatrixMarket File
        void write(const char *filename) const;
    };
}

#endif

