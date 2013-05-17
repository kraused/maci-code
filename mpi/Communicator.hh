
// vi: tabstop=4:expandtab

#ifndef MACI_MPI_COMM_HH_INCLUDED
#define MACI_MPI_COMM_HH_INCLUDED 1

#include "mpi/config.hh"
#include "mpi/Group.hh"
#include "mpi/Irequest.hh"

namespace mpi
{

    /// Communicator: Wrapper around a MPI communicator
    /// A communicator is by definition n order set of processes 
    /// (i.e. a group) and a context
    class Communicator : public Group
    {

    public:
        /// Construct a new (invalid) communicator
        inline Communicator()
        : comm_(MPI_COMM_NULL)    
        {
        }

        /// Create a communicator from a MPI communicator handle
        /// @param comm the communicator handle
        explicit inline Communicator(MPI_Comm comm) 
        : comm_(MPI_COMM_NULL)      
        {
            initialize(comm);
        }

        /// Copy constructor
        inline Communicator(const Communicator& comm)
        : Group(comm), comm_(MPI_COMM_NULL)
        {
            initialize(comm.comm_);
        }

        /// Assignment operator
        /// @param comm the other communicator
        inline Communicator& operator=(const Communicator& comm)
        {
            mpi_comm_free(&comm_);
            initialize(comm.comm_);

            Group::operator=(comm);
            
            return *this;
        }

        /// Destructor
        ~Communicator()
        {
            mpi_comm_free(&comm_);
        }   

        /// Split the communicator
        /// @param color    control of subset assignment
        /// @results        the splitted communicator
        Communicator split(int color) const;

        /// Pt-2-pt send operation
        /// @param buf      send buffer
        /// @param count    number of elements in send buffer
        /// @param datatype datatype of each element in send buffer
        /// @param dest     rank of destination
        /// @param tag      message tag
        void send(void *buf, int count, MPI_Datatype datatype, int dest, int tag) const
        {
            CHECK_PRECOND_ALWAYS((0 == count) || buf);
            CHECK_PRECOND_ALWAYS(count >= 0);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);

            int err = MPI_Send(buf, count, datatype, dest, tag, comm_);
            CHECK_ALWAYS(!err, "MPI call failed");
        }

        /// Pt-2-pt recv operation
        /// @param buf      send buffer
        /// @param count    number of elements in recv buffer
        /// @param datatype datatype of each element in recv buffer
        /// @param dest     rank of destination
        /// @param tag      message tag
        void recv(void *buf, int count, MPI_Datatype datatype, int src, int tag) const
        {
            CHECK_PRECOND_ALWAYS((0 == count) || buf);
            CHECK_PRECOND_ALWAYS(count >= 0);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);

            int err = MPI_Recv(buf, count, datatype, src, tag, comm_, MPI_STATUS_IGNORE);
            CHECK_ALWAYS(!err, "MPI call failed");
        }

        /// Pt-2-pt nonblocking send operation
        /// @param buf      send buffer
        /// @param count    number of elements in send buffer
        /// @param datatype datatype of each element in send buffer
        /// @param dest     rank of destination
        /// @param tag      message tag
        /// @returns        request handle
        Irequest isend(void *buf, int count, MPI_Datatype datatype, int dest, int tag) const
        {
            CHECK_PRECOND_ALWAYS((0 == count) || buf);
            CHECK_PRECOND_ALWAYS(count >= 0);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);

            Irequest req;
            int err = MPI_Isend(buf, count, datatype, dest, tag, comm_, &req);
            CHECK_ALWAYS(!err, "MPI call failed");
            return req;
        }

        /// Pt-2-pt nonblocking recv operation
        /// @param buf      send buffer
        /// @param count    number of elements in recv buffer
        /// @param datatype datatype of each element in recv buffer
        /// @param dest     rank of destination
        /// @param tag      message tag
        /// @returns        request handle
        Irequest irecv(void *buf, int count, MPI_Datatype datatype, int src, int tag) const
        {
            CHECK_PRECOND_ALWAYS((0 == count) || buf);
            CHECK_PRECOND_ALWAYS(count >= 0);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);

            Irequest req;
            int err = MPI_Irecv(buf, count, datatype, src, tag, comm_, &req);
            CHECK_ALWAYS(!err, "MPI call failed");
            return req;
        }

        /// Sendrecv_replace operation: See the MPI standard for explanations
        /// e.g. p. 74 of the MPI 2.1 standard
        /// @param buf      send and receive buffer
        /// @param count    number of elements in send and receive buffer
        /// @param datatype type of elements
        /// @param dest     rank of destination
        /// @param sendtag  send message tag
        /// @param source   rank of source
        /// @param recvtag  receive message tag
        void sendrecv_replace(void* buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag)
        {
            CHECK_PRECOND_ALWAYS((0 == count) || buf);
            CHECK_PRECOND_ALWAYS(count >= 0);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);

            int err = MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm_, MPI_STATUS_IGNORE);
            CHECK_ALWAYS(!err, "MPI call failed");
        }

        /// Allgather operation: See the MPI standard for explanations
        /// e.g. p. 152 of the MPI 2.1 standard
        /// @param sendbuf      starting address of send buffer
        /// @param sendcount    number of elements in send buffer
        /// @param sendtype     data type of send buffer elements
        /// @param recvbuf      address of receive buffer
        /// @param recvcount    number of elements received from any process
        /// @param recvtype     data type of receive buffer elements
        void allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype) const
        {
            CHECK_PRECOND_ALWAYS((sendcount == 0) || sendbuf);
            CHECK_PRECOND_ALWAYS(sendcount >= 0);
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != sendtype);
            CHECK_PRECOND_ALWAYS((recvcount == 0) || recvbuf);
            CHECK_PRECOND_ALWAYS(recvcount >= 0);
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != recvtype);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);

            int err = MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm_);
            CHECK_ALWAYS(!err, "MPI call failed");
        }

        /// Allgatherv operation: See the MPI standard for explanations
        /// e.g. p. 153 of the MPI 2.1 standard
        /// @param sendbuf      starting address of send buffer
        /// @param sendcount    number of elements in send buffer
        /// @param sendtype     data type of send buffer elements
        /// @param recvbuf      address of receive buffer
        /// @param recvcounts   number of elements received from each process
        /// @param recvdispls   displacements at which to place incoming data
        /// @param recvtype data type of receive buffer elements
        void allgatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int* recvcounts, int* recvdispls, MPI_Datatype recvtype) const
        {
            CHECK_PRECOND_ALWAYS((sendcount == 0) || sendbuf);
            CHECK_PRECOND_ALWAYS(sendcount >= 0);
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != sendtype);
            CHECK_PRECOND_ALWAYS(recvcounts);
            CHECK_PRECOND_ALWAYS(recvdispls);
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != recvtype);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);

            int err = MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, recvdispls, recvtype, comm_);
            CHECK_ALWAYS(!err, "MPI call failed");
        }
        
        /// Alltoall operation: See the MPI standard for explanations
        /// e.g. p. 155 of the MPI 2.1 standard
        /// @param sendbuf  starting address of send buffer
        /// @param sendcount    number of elements send to each process
        /// @param sendtype     data type of send buffer elements
        /// @param recvbuf  address of receive buffer
        /// @param recvcount    number of elements received from each process
        /// @param recvtype  data type of receive buffer elements
        void alltoall(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype) const
        {
            CHECK_PRECOND_ALWAYS((0 == sendcount) || sendbuf);
            CHECK_PRECOND_ALWAYS(sendcount >= 0);
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != sendtype);
            CHECK_PRECOND_ALWAYS((0 == recvcount) || recvbuf);
            CHECK_PRECOND_ALWAYS(recvcount >= 0);
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != recvtype);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);

            int err = MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm_);
            CHECK_ALWAYS(!err, "MPI call failed");          
        }

        /// Alltoall operation: See the MPI standard for explanations
        /// e.g. p. 155 of the MPI 2.1 standard
        /// @param sendbuf  starting address of send buffer
        /// @param sendcounts   number of elements send
        /// @param sdispls  displacements
        /// @param sendtype data type of send buffer elements
        /// @param recvbuf  address of receive buffer
        /// @param recvcounts   number of elements received
        /// @param rdispls  displacements
        /// @param recvtype data type of receive buffer elements
        void alltoallv(void *sendbuf, int *sendcounts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *rdispls, MPI_Datatype recvtype) const
        {
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != sendtype);
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != recvtype);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);
            CHECK_PRECOND_ALWAYS(sdispls);
            CHECK_PRECOND_ALWAYS(rdispls);

            int err = MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm_);
            CHECK_ALWAYS(!err, "MPI call failed");          
        }

        /// Allreduce operation: See the MPI standard for explanations
        /// e.g. p. 172
        /// @param sendbuf  starting address of send buffer
        /// @param recvbuf  starting address of receive buffer
        /// @param count    number of elements in send buffer
        /// @param datatype data type of send buffer elements
        /// @param op       operation
        void allreduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op) const
        {
            CHECK_PRECOND_ALWAYS((0 == count) || sendbuf);
            CHECK_PRECOND_ALWAYS((0 == count) || recvbuf);
            CHECK_PRECOND_ALWAYS(count >= 0);
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != datatype);
            CHECK_PRECOND_ALWAYS(MPI_OP_NULL != op);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);           

            int err = MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm_);
            CHECK_ALWAYS(!err, "MPI call failed");              
        }           

        /// Exscan operation: See the MPI standard for explanations
        /// e.g. 
        void exscan(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op)
        {
            CHECK_PRECOND_ALWAYS((0 == count) || sendbuf);
            CHECK_PRECOND_ALWAYS((0 == count) || recvbuf);
            CHECK_PRECOND_ALWAYS(count >= 0);
            CHECK_PRECOND_ALWAYS(MPI_DATATYPE_NULL != datatype);
            CHECK_PRECOND_ALWAYS(MPI_OP_NULL != op);
            CHECK_PRECOND_ALWAYS(MPI_COMM_NULL != comm_);

            int err = MPI_Exscan(sendbuf, recvbuf, count, datatype, op, comm_);
            CHECK_ALWAYS(!err, "MPI call failed");      
        }

        /// Barrier operation
        void barrier() const
        {
            int err = MPI_Barrier(comm_);
            CHECK_ALWAYS(!err, "MPI call failed");          
        }

        /// Access to the handle if this is needed by some class
        /// @returns    handle
        MPI_Comm getHandle() const
        {
            return comm_;
        }

    protected:
        /// The communicator handle
        MPI_Comm    comm_;

        /// Initialize an instance
        /// @param comm the input communicator
        void initialize(MPI_Comm comm)
        {
            if(MPI_COMM_NULL != comm) 
            {
                MPI_Comm_dup(comm, &comm_);
#ifndef NDEBUG
                int flag;
                MPI_Initialized(&flag);
                CHECK_DEBUG(flag, "MPI not initialized");
                MPI_Finalized(&flag);
                CHECK_DEBUG(!flag, "MPI finalized");
#endif

                MPI_Group temp;
                MPI_Comm_group(comm, &temp);
                Group::initialize(temp);
                MPI_Group_free(&temp);

#ifndef NDEBUG
                int rank, numprocs;
                MPI_Comm_rank(comm_, &rank);
                MPI_Comm_size(comm_, &numprocs);
                CHECK_DEBUG(rank == myrank() && numprocs == size(), 0);
#endif
            } 
            else 
                comm_ = MPI_COMM_NULL;
        }
    };

}

#endif

