
// vi: tabstop=4:expandtab

#ifndef MACI_PML_DAMPING_COEFFICIENTS_COMPUTER_HH_INCLUDED
#define MACI_PML_DAMPING_COEFFICIENTS_COMPUTER_HH_INCLUDED 1

#include "Function.hh"

/// DampingCoefficientsComputer: Comptutes the damping coefficients
///              e.g. for a PML method
struct DampingCoefficientsComputer
{

    /// Compute the dampingD values of the piggyback data
    /// These are the values of the damping function at the
    /// reference positions
    /// @param[in]  D       The function
    static void ComputeCoefficients(const Function<double,3>* D);

    /// Write the dampingD values to a file
    static void WriteCoefficients();

};

#endif

