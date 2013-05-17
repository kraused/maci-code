
// vi: tabstop=4:expandtab

#ifndef MACI_MESH_MASS_ASSEMBLER_HH_INCLUDED
#define MACI_MESH_MASS_ASSEMBLER_HH_INCLUDED 1

#include "Assembler.hh"
#include "Function.hh"
#include "fe/FiniteElements.hh"


/// MeshMassAssembler: Base class for all mesh
///                    mass assembler
class MeshMassAssembler : public Assembler
{
protected:
    typedef fe::interface::Element Element;
    typedef fe::interface::Node    Node;

public:
    /// Assemble the local element mass matrix for
    /// one element. The function takes the element
    /// and the density as first and second argument
    /// In the last argument the output matrix is 
    /// returned as a [ncorners] x [ncorners] matrix
    /// in row-major format.
    void AssembleOnElement(const Element& el, const Function<double, 3>& f, double* localMat);

};

#endif

