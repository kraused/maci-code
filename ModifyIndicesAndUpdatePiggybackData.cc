
// vi: tabstop=4:expandtab

#include "ModifyIndicesAndUpdatePiggybackData.hh"


void GatherUniqueColumnIndices(linalg::CoordMatrix<int64,double>& A, std::vector<int64>* columnIndices)
{
    std::size_t i;
    std::vector<int64>::iterator it;

    columnIndices->reserve(A.size());
    for(i = 0; i < A.size(); ++i)
        columnIndices->push_back(A[i].col);

    std::sort(columnIndices->begin(), columnIndices->end());
    it = std::unique(columnIndices->begin(), columnIndices->end());
    columnIndices->erase(it, columnIndices->end());
}

