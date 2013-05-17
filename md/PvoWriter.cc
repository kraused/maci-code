
// vi: tabstop=4:expandtab

#include <mpi.h>

#include "md/PvoWriter.hh"
#include "md/MolecularDynamics.hh"

using namespace md::interface;


PvoWriter::PvoWriter(System* system)
{
    pvo_init( system->GetCommunicator() );

    pvo_cookie_create( 0/*pvo_world_rank()*/, &cookie);
}


PvoWriter::~PvoWriter()
{
    pvo_quit();
}


void PvoWriter::Write(const std::string& file, System* system)
{
    pvo_float3_t* pts;
    double* u;
    double* v;
    int*    ranks;
    long    i;  
    int     k;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    pts   = new pvo_float3_t[system->NlocalAtoms()];
    u     = new double[3*system->NlocalAtoms()];
    v     = new double[3*system->NlocalAtoms()];
    ranks = new int[system->NlocalAtoms()];

    system->AccessBegin(System::RD_REFP | System::RD_POS | System::RD_VEL);

    i = 0;
    for(ParticleHandle ph = system->begin(); ph != system->end(); ++ph)
    {
        for(k = 0; k < DIM; ++k)
            pts[i][k] = (float )Property::RefP(*system, ph, k);
        if(2 == DIM)
            pts[i][2] = 0.0;

        for(k = 0; k < DIM; ++k)
            u[3*i+k] = Property::Displ(*system, ph, k);
        if(2 == DIM)
            u[3*i+k] = 0.0;

        for(k = 0; k < DIM; ++k)
            v[3*i+k] = Property::Vel(*system, ph, k);
        if(2 == DIM)
            v[3*i+k] = 0.0;

        ranks[i] = rank;

        ++i;
    }

    pvo_cookie_insert_var(cookie, PVO_VAR_NODEDATA, PVO_VAR_FLOAT64, 3, "u"    , u);
    pvo_cookie_insert_var(cookie, PVO_VAR_NODEDATA, PVO_VAR_FLOAT64, 3, "v"    , v);
    pvo_cookie_insert_var(cookie, PVO_VAR_NODEDATA, PVO_VAR_INT32  , 1, "ranks", ranks);

    pvo_vtp_file_open(file.c_str(), cookie, system->NlocalAtoms(), pts, &fo);
    pvo_file_write((pvo_file_t )fo);
    pvo_vtp_file_close(fo);

    pvo_cookie_remove_var(cookie, "u");
    pvo_cookie_remove_var(cookie, "v");
    pvo_cookie_remove_var(cookie, "ranks");

    system->AccessEnd();

    delete[] pts;
    delete[] u;
    delete[] v;
    delete[] ranks;
}


template<typename Tp>
static void WriteArrayGeneric(const std::string& file, System* system, Tp* a, const std::string& varname, 
                              pvo_var_type_t type, pvo_cookie_t cookie, pvo_vtp_file_t fo)
{
    pvo_float3_t* pts;
    int*    ranks;
    long    i;
    int     k;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    pts   = new pvo_float3_t[system->NlocalAtoms()];
    ranks = new int[system->NlocalAtoms()];

    system->AccessBegin(System::RD_REFP | System::RD_POS | System::RD_VEL);

    i = 0;
    for(ParticleHandle ph = system->begin(); ph != system->end(); ++ph)
    {
        for(k = 0; k < DIM; ++k)
            pts[i][k] = (float )Property::RefP(*system, ph, k);
#if 2 == DIM
        pts[i][2] = 0.0;
#endif
        ranks[i] = rank;

        ++i;
    }

    system->AccessEnd();

    pvo_cookie_insert_var(cookie, PVO_VAR_NODEDATA, type,            1, varname.c_str(), a);
    pvo_cookie_insert_var(cookie, PVO_VAR_NODEDATA, PVO_VAR_INT32  , 1, "ranks"        , ranks);

    pvo_vtp_file_open(file.c_str(), cookie, system->NlocalAtoms(), pts, &fo);
    pvo_file_write((pvo_file_t )fo);
    pvo_vtp_file_close(fo);

    delete[] pts;
    delete[] ranks;
}


void PvoWriter::WriteArrayInt   (const std::string& file, System* system, int   * a, const std::string& varname)
{
    WriteArrayGeneric(file, system, a, varname, PVO_VAR_INT32, cookie, fo);
}


void PvoWriter::WriteArrayFloat (const std::string& file, System* system, float * a, const std::string& varname)
{
    WriteArrayGeneric(file, system, a, varname, PVO_VAR_FLOAT32, cookie, fo);
}


void PvoWriter::WriteArrayDouble(const std::string& file, System* system, double* a, const std::string& varname)
{
    WriteArrayGeneric(file, system, a, varname, PVO_VAR_FLOAT64, cookie, fo);
}


