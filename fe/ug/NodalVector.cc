
// vi: tabstop=4:expandtab

#include "NodalVector.hh"
#include "Solver.hh"

#include <stdint.h>

using namespace fe::interface;

template<typename Tp, int N>
void NodalVector<Tp, N>::ParallelCopy (Solver& solver, const NodeDistributionMap* map)
{
    solver.GetImplementation()->ParallelCopy <Tp, N>(static_cast<fe::ug::NodalVector<DIM>*>(this), static_cast<const fe::ug::NodalVector<DIM>*>(map), map->rank());
}

template<typename Tp, int N>
void NodalVector<Tp, N>::ParallelSumup(Solver& solver, const NodeDistributionMap* map)
{
    solver.GetImplementation()->ParallelSumup<Tp, N>(static_cast<fe::ug::NodalVector<DIM>*>(this));
}

template<typename Tp, int N>
void NodalVector<Tp, N>::ParallelMax  (Solver& solver, const NodeDistributionMap* map)
{
    solver.GetImplementation()->ParallelMax  <Tp, N>(static_cast<fe::ug::NodalVector<DIM>*>(this));
}

/* The class must be explicitly initialized since we implement
   some of the functions in the .cc file */
template class NodalVector<double, 1>;
template class NodalVector<double, 2>;
template class NodalVector<double, 3>;

template class NodalVector<int, 1>;
template class NodalVector<int, 2>;
template class NodalVector<int, 3>;

template class NodalVector<int64_t, 1>;
template class NodalVector<int64_t, 2>;
template class NodalVector<int64_t, 3>;

