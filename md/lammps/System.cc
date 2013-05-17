
// vi: tabstop=4:expandtab

#include "md/lammps/System.hh"
#include "md/Writer.hh"

#include <assert.h>


using namespace md::interface;

System::System(Implementation& Imp)
: Base(Imp), cartworld(GetImplementation()->createCartWorld()), writer(0)
{
}

System::~System()
{
    MPI_Comm_free(&cartworld);

    if(writer)
        delete writer;
}

ParticleHandle System::begin() const
{
    return ParticleHandle(0);
}

ParticleHandle System::end() const
{
    return ParticleHandle(NlocalAtoms());
}

void System::GetSpatialRegion(double* lower, double* upper) const
{
    if(not GetImplementation()->getSpatialRegionIfBox(lower, upper)) 
    {
        /// TODO Die here
        exit(128);
    }
}

void System::Write(const std::string& file)
{
    if(0 == writer)
        writer = Writer::New(this);
    ASSERT(writer);
    
    writer->Write(file, this);
}

void System::AccessBegin(int type)
{
    epoch_type = type;
    this->GetImplementation()->AccessBegin(type);
}

void System::AccessEnd()
{
    this->GetImplementation()->AccessEnd();
    epoch_type = INVALID;
}

bool System::CanAccess(int* type)
{
    if(type)
        *type = epoch_type;
    return this->GetImplementation()->CanAccess();
}

