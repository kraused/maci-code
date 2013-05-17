
// vi: tabstop=4:expandtab

#include "fe/ug/Solver.hh"

/* The MACI config file. Needed to
   know whether we can use PVO */
#include <config.hh>

#ifdef HAVE_PVO
#include "fe/PvoWriter.hh"
#endif

#include <assert.h>


using namespace fe::interface;


Solver::Solver(Implementation& Imp)
: Base(Imp), mesh(Imp.GetMesh()), epoch_type(INVALID), distrib_map(&mesh), writer(0)
{
}

Solver::~Solver()
{
    if(writer)
        delete writer;
}

void Solver::AccessBegin(int type)
{
    if(INVALID != epoch_type)
    {
        fprintf(stderr, "ERROR: INVALID != epoch_type");
        exit(128);  /* TODO This code should be able to give
                   better error handling (via exceptions).
                   Can we use DesignByContract in here? */
    }

    epoch_type = type;
}

void Solver::AccessEnd()
{
    if(WR_VEL  & epoch_type)
        GetImplementation()->MakeConsistentV(&distrib_map, distrib_map.rank());

    if(WR_DISP & epoch_type)
        GetImplementation()->MakeConsistentU(&distrib_map, distrib_map.rank());

    epoch_type = INVALID;
}

void Solver::Write(const std::string& file)
{
#ifdef HAVE_PVO
    if(0 == writer)
        writer = new PvoWriter(this);
#endif
    ASSERT(writer);
    writer->Write(file, this);
}


extern MPI_Comm UG_COMM_WORLD;

MPI_Comm Solver::GetCommunicator()
{
    return UG_COMM_WORLD;
}

