//
/// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#ifndef MACI_MPI_GROUP_HH_INCLUDED
#define MACI_MPI_GROUP_HH_INCLUDED 1

#include "mpi/config.hh"

namespace mpi {
    
    /// Group: Wrapper class around an MPI group
    /// Represents an orders set of processes
    class Group {

    public:
        /// Construct a new (invalid) group
        inline Group()
        : grp_(MPI_GROUP_NULL), myrank_(MPI_UNDEFINED), numprocs_(0)          
        {
            CHECK_POSTCOND_ALWAYS(MPI_UNDEFINED == myrank_ ||
                (0 <= myrank_ && myrank_ < numprocs_));
        }

        /// Create a group from a MPI group handle
        /// @param grp  the group handle
        inline Group(MPI_Group grp) 
        : grp_(MPI_GROUP_NULL), myrank_(MPI_UNDEFINED), numprocs_(0)    
        {
            initialize(grp);
            CHECK_POSTCOND_ALWAYS(MPI_UNDEFINED == myrank_ ||
                (0 <= myrank_ && myrank_ < numprocs_));
        }

        /// Create a new group by including only processes in a list
        /// @param[in]  grp the original group
        /// @param[in]  n   size of the new group
        /// @param[in]  ranks   array of size n containing the ranks of
        ///         processes to be included in the new group
        inline Group(const Group& grp, int n, const int *ranks)
        : grp_(MPI_GROUP_NULL), myrank_(MPI_UNDEFINED), numprocs_(0)
        {
            MPI_Group newgrp;
            MPI_Group_incl(grp.grp_, n, (int *)ranks, &newgrp);

            initialize(newgrp);
            CHECK_POSTCOND_ALWAYS(MPI_UNDEFINED == myrank_ ||
                (0 <= myrank_ && myrank_ < numprocs_));

            mpi_group_free(&newgrp);
        }

        /// Copy constructor
        inline Group(const Group& grp)
        : grp_(MPI_GROUP_NULL), myrank_(MPI_UNDEFINED), numprocs_(0)
        {
            initialize(grp.grp_);

            CHECK_DEBUG(myrank_ == grp.myrank_, "unexpected behaviour");
            CHECK_DEBUG(numprocs_ == grp.numprocs_, "unexpected behaviour");
            CHECK_POSTCOND_ALWAYS(MPI_UNDEFINED == myrank_ ||
                (0 <= myrank_ && myrank_ < numprocs_));
        }

        /// Returns true of the process is a member of the group
        /// @returns int = 0, 1
        inline int ismember() const
        {
            CHECK_ALWAYS(MPI_GROUP_NULL != grp_, 0);
            return (MPI_UNDEFINED != myrank_);
        }

        /// Assignment operator
        /// @param grp  the other group
        inline Group& operator=(const Group& grp)
        {
            mpi_group_free(&grp_);
            initialize(grp.grp_);           

            CHECK_DEBUG(myrank_ == grp.myrank_, "unexpected behaviour");
            CHECK_DEBUG(numprocs_ == grp.numprocs_, "unexpected behaviour");

            return *this;
        }   

        /// Destructor
        ~Group()
        {
            mpi_group_free(&grp_);
        }

        /// Get the difference of this group and the argument
        /// @param grp  the group to exclude
        /// @returns    difference
        Group operator-(const Group& grp);

        /// Get rank of PE in group
        /// @returns rank >= 0 or MPI_UNDEFINED
        inline int myrank() const
        {
            return myrank_;
        }

        /// Get size of group
        /// @returns size >= 0
        inline int size() const
        {
            CHECK_DEBUG(numprocs_ >= 0, 0);
            return numprocs_;
        }

        /// Translate ranks from one group to another
        /// @param from the group to transform from
        /// @param p    rank of the process in the group from
        /// @param to   the group to translate to
        static inline int translate(const Group& from, int p, 
                        const Group& to)
        {
            int q;

            MPI_Group_translate_ranks(from.grp_, 1, &p, to.grp_, &q);
            if(MPI_UNDEFINED == q)
                q = MPI_PROC_NULL;

            return q;
        }

    protected:
        /// The MPI group
        MPI_Group grp_;
        /// Myrank in the group if I'm a member or MPI_UNDEFINED
        /// otherwise
        int myrank_;
        /// Number of processes in the group
        int numprocs_;

        /// Initialize an instance
        /// @param grp  the group handle    
        inline void initialize(MPI_Group grp)
        {
            int err;

            if(MPI_GROUP_NULL != grp) {
                err = MPI_Group_union(grp, 
                        MPI_GROUP_EMPTY, &grp_);
                CHECK_ALWAYS(!err, "MPI Call failed");

                err = MPI_Group_rank(grp_, &myrank_);
                CHECK_ALWAYS(!err, "MPI Call failed");

                err = MPI_Group_size(grp_, &numprocs_);
                CHECK_ALWAYS(!err, "MPI Call failed");

                CHECK_ALWAYS(MPI_UNDEFINED == myrank_ ||
                    (0 <= myrank_ && myrank_ < numprocs_), 0);              

#ifndef NDEBUG
                int size;
                MPI_Group_size(grp, &size);
                CHECK_DEBUG(size == numprocs_, 0);
                int rk;
                MPI_Group_rank(grp, &rk);
                CHECK_DEBUG(rk == myrank_, 0);
#endif
            } else {
                grp_      = MPI_GROUP_NULL;
                myrank_   = MPI_UNDEFINED; 
                numprocs_ = 0;
            }
        }
    };
}

#endif

