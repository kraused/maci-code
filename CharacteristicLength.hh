
// vi: tabstop=4:expandtab

#ifndef MACI_CHARACTERISTIC_LENGTH_HH_INCLUDED
#define MACI_CHARACTERISTIC_LENGTH_HH_INCLUDED 1

/// CharacteristicLength: Define a characteristic length scale for a simulation
/// A problem which often pops up when dealing with computational geometry type
/// operations (i.e. checking whether a point lies in a polytope) is the definition
/// of appropriate tolerances. To make the software robust the tolerance must
/// be chosen relative the the "length scale" of the simulation. This class 
/// defines the length scale of the simulation in a consistent fashion given
/// access to the value on each processing element.
template<typename Float>
class CharacteristicLength
{
    typedef CharacteristicLength<Float> CL;

public:
    /// Create a characteristic length. This 
    CharacteristicLength(Float v);

    /// Dereferencing operator gives (r.o.) access to the characteristic
    /// length
    inline Float operator*() const
    {
        return val;
    }

private:
    static CL* ui;  //< Unique instance
    Float      val; //< The characteristic length
};

#endif


