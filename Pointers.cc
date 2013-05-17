
// vi: tabstop=4:expandtab

#include "Pointers.hh"
#include "Maci.hh"

Pointers::Pointers(Maci* ptr)
: app(ptr), solver(ptr->solver), system(ptr->system), procel(ptr->procel), 
  weights(ptr->weights), handshake(ptr->handshake), plugins(ptr->plugins),
  integrator(ptr->integrator), memory(ptr->memory)
{

}

