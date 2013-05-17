
// vi: tabstop=4:expandtab

#include "puBasisFunction.hh"

namespace pulib
{

template<int D>
void Basis<D>::printStatistic() const
{
    int   min = begin()->getNumOvPatches();
    int   max = begin()->getNumOvPatches();
    float avg = 0;

    for(ConstIterator it = begin(); it != end(); ++it) {
        min  = std::min(min, it->getNumOvPatches());
        max  = std::max(max, it->getNumOvPatches());
        avg += it->getNumOvPatches();
    }
    
    avg /= (float )size();
    
    printf(" min = %5d, avg = %8.4f, max = %5d\n", min, avg, max);
}

template
void Basis<2>::printStatistic() const;

template
void Basis<3>::printStatistic() const;

}

