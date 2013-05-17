
// vi: tabstop=4:expandtab

#ifndef MACI_WEIGHTS_HH_INCLUDED
#define MACI_WEIGHTS_HH_INCLUDED 1

#include "Function.hh"
#include "exception/BadCallException.hh"

// Forwarding
class ProcElement;


/// Manages the weighting function
class Weights : public Function<double,3>
{

public:
    /// Cache the values at lattice sites. The data ist stored
    /// in the piggyback data of the md interface
    /// This function should be called in the constructor of the derived
    /// class after everything is set up correctly
    void Update(ProcElement& here);

    // @overrrides
    Function<double,3>* Clone() const
    {
        throw exception::BadCallException();
        return 0;
    }
    
    // Shut up ...
    virtual ~Weights() { }

private:
    /// Write the weights to a file
    void WriteFile();

};

#endif

