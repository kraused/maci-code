
// vi: tabstop=4:expandtab

#include "TreeAssembler.hh"
#include "Delete.hh"
#include "CoordExchange.hh"
#include "DesignByContract.hh"
#include "ProcElement.hh"
#include "Logging.hh"


TreeAssembler::TreeAssembler(const HandshakeGeo& hs, const float h[])
: elements(0), nodes(0), ellist(0), puspace(hs, h), imat(0)
{
    if(PROC_FE == gcpe()->mytype) 
    {
        elements = new ElementList(hs);
        nodes    = new NodeList(hs, SelectAllNodes());
    }

    if(elements)
        ellist = new ElementShadowList(*elements);
    else
        ellist = new ElementShadowList();
    ellist->Exchange();
}

TreeAssembler::~TreeAssembler()
{
    DeletePointer(elements);
    DeletePointer(nodes);
    DeletePointer(ellist);
    DeletePointer(imat);
}

void TreeAssembler::AssembleIntermediateMatrix()
{
    CHECK_PRECOND_ALWAYS(ellist);
    CHECK_PRECOND_ALWAYS(!imat);    /// Otherwise we would assemble twice

    imat = new TreeAssemblerIntermediateFormat<double>(*ellist);

    if(PROC_MD == gcpe()->mytype)
    {
        int i;

        i = 0;
        for(ElementShadowList::Iterator el = ellist->begin(); el != ellist->end(); ++el)
            imat->reserve(i++, QueryStorageRequirements(el));

        for(ElementShadowList::Iterator el = ellist->begin(); el != ellist->end(); ++el)
            BuildIntermediateMatrix(el, imat);
    }
}

void TreeAssembler::DeleteIntermediateMatrix()
{
    DeletePointer(imat);
}

void TreeAssembler::CreateConsistentCoordinateMatrix()
{
    imat->Exchange();

    if(PROC_FE == gcpe()->mytype)
    {
        long index;
        linalg::CoordMatrix<int64, double> recvbuf, sendbuf;
        std::size_t i;

        typedef fe::interface::Node Node;
        typedef TreeAssemblerIntermediateFormat<double>::Iterator Iterator;


        for(ElementShadowList::ConstIterator el = ellist->begin(); el != ellist->end(); ++el)
        {
            for(Iterator it = imat->begin(*el); it != imat->end(*el); ++it)
            {
                Node nd = (*elements)[it->elementIndex].corner(it->cornerIndex);
                index   = Node::Index(*gcpe()->GetFESolver(), nd);

                /// The output matrices of the assembler are stored non-overlapping
                /// therefore we insert only the rows for those nodes we own and
                /// 
                if(gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(nd))
                    matrix.insert(Int64::concat(gcpe()->world.myrank(), index), it->atomIndex, it->val);
                else
                    sendbuf.insert(index, it->atomIndex, it->val);
            }
            HandleDuplicates(matrix);
        }
        HandleDuplicates(sendbuf);

        /// Exchange the data at the borders
        GatherRowsAtOwners(sendbuf, recvbuf);
        HandleDuplicates(recvbuf);

        for(i = 0; i < recvbuf.size(); ++i)
        {
            CHECK_DEBUG(0 == Int64::hi(recvbuf[i].row), 0);
            matrix.insert(Int64::concat(gcpe()->world.myrank(), Int64::lo(recvbuf[i].row)), recvbuf[i].col, recvbuf[i].val);
        }
        HandleDuplicates(matrix);

        for(i = 0; i < matrix.size(); ++i)
        {
            bool found = 0;
            for(Node nd = gcpe()->GetFESolver()->GetMesh()->NodesBegin(); nd != gcpe()->GetFESolver()->GetMesh()->NodesEnd(); ++nd)
            {
                if(not gcpe()->GetFESolver()->GetNodeDistribMap()->bOwn(nd))
                    continue;

                if(Int64::lo(matrix[i].row) == Node::Index(*gcpe()->GetFESolver(), nd))
                {
                    found = 1;
                    break;
                }
            }
            CHECK_ALWAYS(found, "wrong matrix");
        }
    }
    
    gcpe()->world.barrier();
    Log::Instance().write("Finished CreateConsistentCoordinateMatrix()\n");    
}

