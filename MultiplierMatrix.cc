
// vi: tabstop=4:expandtab

#include "MultiplierMatrix.hh"
#include "Delete.hh"
#include "Maci.hh"


MultiplierMatrix::MultiplierMatrix(Maci* app)
: Pointers(app), solver(0)
{
}

MultiplierMatrix::~MultiplierMatrix()
{
    DeletePointer(solver);
}

