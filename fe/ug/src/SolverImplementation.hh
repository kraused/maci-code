
// vi: tabstop=4:expandtab

#ifndef MACI_FE_UG_SRC_SOLVER_IMPLEMENTATION_HH_INCLUDED
#define MACI_FE_UG_SRC_SOLVER_IMPLEMENTATION_HH_INCLUDED 1

#include <string>

#include "fe/ug/src/DirichletEnforcer.hh"

#include "ugNodalVector.hh"
#include "ugNodePointer.hh"
#include "ugMesh.hh"
#include "ugS1MassMatrix.hh"
#include "ugEnergyCalculator.hh"
#include "ugMetaForceCalculator.hh"


namespace UG_IF
{

    /// SolverImplementation: Helper class used to encapsule
    ///               UG functionality
    class SolverImplementation
    {
    /* The typedefs need to be public so that SWIG can access
       them */
    public:
        /// Short-cuts for the UG classes
        typedef fe::ug::Mesh<DIM>                   Mesh;
        typedef fe::ug::NodePointer<DIM>            NodePointer;
        typedef fe::ug::NodalVector<DIM>            NodalVector;
        typedef fe::ug::S1MassMatrix<DIM>           LumpedMassMatrix;
        typedef fe::ug::MetaForceCalculator<DIM>    ForceCalc;
        typedef fe::ug::EnergyCalculator<DIM>       EnergyCalc;

        /// Create a new instance given a pointer to the
        /// mesh. The pointer must be valid throughout the
        /// lifetime of this class
        SolverImplementation(Mesh* M);

        /// Destroy the instance
        ~SolverImplementation();


        /// Return the displacement vector at a node in a
        /// specific direction (const version)
        /// @param[in]  nd  the node
        /// @param[in]  i   direction (between zero and DIM-1)
        double  U(const NodePointer& nd, int i) const
        {
            return u[nd][i];
        }

        /// Return the displacement vector at a node in a
        /// specific direction
        /// @param[in]  nd      the node
        /// @param[in]  i       direction (between zero and DIM-1)
        double& U(NodePointer& nd, int i)
        {
            return u[nd][i];
        }

        /// Return the velocity vector at a node in a
        /// specific direction (const version)
        /// @param[in]  nd      the node
        /// @param[in]  i       direction (between zero and DIM-1)
        double  V(const NodePointer& nd, int i) const
        {
            return v[nd][i];
        }

        /// Return the velocity vector at a node in a
        /// specific direction
        /// @param[in]  nd      the node
        /// @param[in]  i       direction (between zero and DIM-1)
        double& V(NodePointer& nd, int i)
        {
            return v[nd][i];
        }

        /// Return the force vector at a node in a specific
        /// direction (read only)
        /// @param[in]  nd      the node
        /// @param[in]  i       direction (between zero and DIM-1)
        double  F(const NodePointer& nd, int i) const
        {
            return (*forcecalc)[nd][i];
        }

        /// Return the diagonal entry of the unweighted mass 
        /// matrix at a node
        /// @param[in]  nd      the node
        double  UnweightedM(const NodePointer& nd) const
        {
            return (*umat)(nd,nd)[0];
        }

        /// Return the diagonal entry of the weighted mass matrix 
        /// at a node
        /// @param[in]  nd      the node
        double  WeightedM(const NodePointer& nd) const
        {
            return (*wmat)(nd,nd)[0];
        }

        /// Return the diagonal entry of the mass matrix at
        /// a node. Here "mass matrix" refers to the mass matrix
        /// used in the time integrator
        /// @param[in]  nd      the node
        double  M(const NodePointer& nd) const
        {
            return (*imat)(nd,nd)[0];
        }

        /// Get a pointer to the finite element mesh
        Mesh* GetMesh()
        {
            return mesh;
        }

        /// Copy from owned nodes non-owned nodes on the other processing
        /// elements
        /// @param[in]  t   node vector
        /// @param[in]  map node-to-owner mapping
        /// @param[in]  here    rank of the processing element. This value
        ///         is compared to the content of map to check
        ///         if the processing element owns a node
        template<typename Tp, int N>
        void ParallelCopy (NodalVector* t, const NodalVector* map, int here);

        /// Sum-up the nodal values at shared nodes
        /// @param[in]  t       node vector
        template<typename Tp, int N>
        void ParallelSumup(NodalVector* t);

        /// Compute the maximal values at shared nodes
        /// @param[in]  t       node vector
        template<typename Tp, int N>
        void ParallelMax  (NodalVector* t);

        /// Make the displacements consistent after local modifications
        void MakeConsistentU(NodalVector* map, int here)
        {
            ParallelCopy<double, 3>(&u, map, here);
            u.CopyBorderToHalo();
        }

        /// Make the velocities consistent after local modifications
        ///
        /// Note: The copy of the velocities to the halo is not really
        ///       necessary since the velocities are never access for
        ///       halo nodes but we do it nevertheless to provide full
        ///       support for consistency. This can be modified later
        ///       to increase scalability.
        /// 
        void MakeConsistentV(NodalVector* map, int here)
        {
            ParallelCopy<double, 3>(&v, map, here);
            v.CopyBorderToHalo();
        }

        /// Compute the kinetic energy of the system
        /// @returns    the kinetic energy value
        double KineticEnergy();

        /// Compute the potential energy of the system
        double PotentialEnergy();

        /// Update the forces
        /// @param[in]  currtime    the current time
        void CalculateForces(double currtime);
        
        /// Quick and dirty way to pass the current time to the force
        /// calculators. This variable should be only read!
        double currtime;

        /// Enforce Dirichlet boundary conditions (primary boundary conditions)
        /// @param[in]  currtime    the current time
        void EnforceDirichletBC(double currtime);

    /* Public so that we can extend the class in SolverImplementation.i */
    public:
        /// Pointer to the mesh.
        Mesh* mesh;

        /// Displacement, velocity and force
        NodalVector u, v/*, f*/;

        /// Unweighted mass matrix. The UG interface (mscoupling) only supports
        /// lumped matrices
        LumpedMassMatrix* umat;

        /// Weighted mass matrix. The UG interface (mscoupling) only supports
        /// lumped matrices
        LumpedMassMatrix* wmat;

        /// The mass matrix used in the time integration 
        LumpedMassMatrix* imat;

        /// The force calculator. This is actually a "MetaForceCalculator"
        /// which means that it allows to plugin different force terms and
        /// sum them up.
        ForceCalc* forcecalc;

        /// DirichletEnforcer: This instance is responsible for 
        DirichletEnforcer* direnforcer;

        /// The calculator for the potential energy. We compute the kinetic
        /// energy on our own
        EnergyCalc* potecalc;
    };

}

#endif

