//
// Copyright (c) 2010 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#if 2 == DIM
%module maciS2
#else
%module maciS3
#endif
%{
#include "fe/ug/src/SolverImplementation.hh"

/* Files from the UG/mscoupling folder that must
   be included so that SWIG can create converters */
#include "ugKineticEnergyCalculator.hh"

#include "python/ugFunctionPyObj.hh"

%}


%ignore UG_IF::SolverImplementation::U const;
%ignore UG_IF::SolverImplementation::V const;
%ignore UG_IF::SolverImplementation::u;
%ignore UG_IF::SolverImplementation::v;
%ignore UG_IF::SolverImplementation::f;
%ignore UG_IF::SolverImplementation::mesh;
%ignore UG_IF::SolverImplementation::direnforcer;

%include "fe/ug/src/SolverImplementation.hh"

%extend UG_IF::SolverImplementation
{

    /* Access functions for the displacements and the velocities
       These functions are needed for the implementation of the
       FEToolkit function on the Python level */
    SolverImplementation::NodalVector& GetU()
    {
        return self->u;
    }

    SolverImplementation::NodalVector& GetV()
    {
        return self->v;
    }

    /* Access fucntion for the mass matrix */
    SolverImplementation::LumpedMassMatrix& GetMassMat()
    {
        return *self->imat;
    }

    /* Set the calculator for the potential energy
       @param[in]  calc    The calculator for the energies
     */
    void SetEnergyCalc(EnergyCalc* calc)
    {
        self->potecalc = calc->clone();
    }

    /* Add a new force calculator
     */
    void AddForceCalc(fe::ug::ForceCalculator<DIM>* calc)
    {
        self->forcecalc->addCalculator(*calc);
    }

    /* Set the dirichlet enforcer variable
     */
    void SetDirichletEnforcer(UG_IF::DirichletEnforcer* direnforcer)
    {
        self->direnforcer = direnforcer;
    }

    /// Assemble the unweighted lumped mass matrix for the FE system.
    /// @param[in]  rho     The density/weighting function
    /// @param[in]  qo      Quadrature order
    void AssembleUnweightedMassMatrix(const fe::Function& rho, int qo)
    {
        self->umat = new UG_IF::SolverImplementation::LumpedMassMatrix(*self->mesh, rho, qo);
    }

    /// Assemble the weighted lumped mass matrix for the FE system. 
    /// @param[in]  rho     The density/weighting function
    /// @param[in]  qo      Quadrature order
    void AssembleWeightedMassMatrix(const fe::Function& rho, int qo)
    {
        self->wmat = new UG_IF::SolverImplementation::LumpedMassMatrix(*self->mesh, rho, qo);
    }

};

