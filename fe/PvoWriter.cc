
// vi: tabstop=4:expandtab

#include <mpi.h>

#include "fe/PvoWriter.hh"
#include "fe/FiniteElements.hh"


using namespace fe::interface;


/// Get the element type given the number of corners
uint8_t GetType(int ncorners)
{
    if(2 == DIM)
    {
        switch(ncorners)
        {
        case 3:
            return PVO_VTU_TRIANGLE;
            break;
        case 4:
            return PVO_VTU_QUADRILATERAL;
            break;
        }
    }
    else
    {

        switch(ncorners)
        {
        case 4:
            return PVO_VTU_TETRAHEDRON;
            break;
        case 8:
            return PVO_VTU_HEXAHEDRON;
            break;
        }
    }

    fprintf(stderr, "ERROR: Cannot assign element type (%d corners).", ncorners);
    exit(128);

    return 0;
}


PvoWriter::PvoWriter(Solver* solver)
{
    long i, N;
    int  k;

    pvo_init(solver->GetCommunicator());

    // Rank of the process in the world communicator. Note that we
    // do not use the UG_COMM_WORLD communicator here because having
    // the rank in MPI_COMM_WORLD allows to view MD and FE ranks in
    // the same picture without need for shifting.
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    pvo_cookie_create( 0/*pvo_world_rank()*/, &cookie);

    buf_u = new double[3*solver->GetMesh()->nnodes()];
    buf_v = new double[3*solver->GetMesh()->nnodes()];
    ranks = new int   [  solver->GetMesh()->nelements()];

    pvo_cookie_insert_var(cookie, PVO_VAR_NODEDATA, PVO_VAR_FLOAT64, 3, "u"    , buf_u);
    pvo_cookie_insert_var(cookie, PVO_VAR_NODEDATA, PVO_VAR_FLOAT64, 3, "v"    , buf_v);
    pvo_cookie_insert_var(cookie, PVO_VAR_CELLDATA, PVO_VAR_INT32  , 1, "ranks", ranks);

    pts = new pvo_float3_t[solver->GetMesh()->nnodes()];
    cia = new int[solver->GetMesh()->nelements()+1];

    N = 0;
    for(Element el = solver->GetMesh()->ElementsBegin(); el != solver->GetMesh()->ElementsEnd(); ++el)
        N += el.ncorners();

    cja   = new int[N];
    types = new uint8_t[solver->GetMesh()->nelements()];

    NodalVector<int> t(solver->GetMesh());

    i = 0;
    for(Node nd = solver->GetMesh()->NodesBegin(); nd != solver->GetMesh()->NodesEnd(); ++nd)
    {
        std::copy(nd.coords(), nd.coords()+DIM, pts[i]);
        if(2 == DIM)
            pts[i][2] = 0;

        t(nd) = i++;
    }

    cia[0] = 0;
    
    i = 0;
    for(Element el = solver->GetMesh()->ElementsBegin(); el != solver->GetMesh()->ElementsEnd(); ++el)
    {
        for(k = 0; k < el.ncorners(); ++k)
            cja[cia[i] + k] = t(el.corner(k));
        cia[i+1] = cia[i] + el.ncorners();

        types[i] = GetType(el.ncorners());
        ranks[i] = rank;

        ++i;
    }

    ASSERT(cia[solver->GetMesh()->nelements()] == N);
}

PvoWriter::~PvoWriter()
{
    pvo_cookie_delete(cookie);

    delete[] buf_u;
    delete[] buf_v;
    delete[] ranks;
    delete[] pts;
    delete[] cia;
    delete[] cja;
    delete[] types;

    pvo_quit();
}

void PvoWriter::Write(const std::string& file, Solver* solver)
{
    long i;
    int  k;

    solver->AccessBegin(Solver::RD_DISP | Solver::RD_VEL);

    i = 0;
    for(Node nd = solver->GetMesh()->NodesBegin(); nd != solver->GetMesh()->NodesEnd(); ++nd)
    {
        for(k = 0; k < DIM; ++k)
            buf_u[3*i+k] = StateVariables::Displ(*solver, nd, k);
        if(2 == DIM)
            buf_u[3*i+2] = 0.0;

        for(k = 0; k < DIM; ++k)
            buf_v[3*i+k] = StateVariables::Vel  (*solver, nd, k);
        if(2 == DIM)
            buf_v[3*i+2] = 0.0;

        ++i;
    }

    pvo_vtu_file_open(file.c_str(), cookie, 
                      solver->GetMesh()->nnodes()   , pts, 
                      solver->GetMesh()->nelements(), cia, cja, types, &fo);
    pvo_file_write((pvo_file_t )fo);
    pvo_vtu_file_close(fo);

    solver->AccessEnd();
}

template<typename Tp>
static void WriteArrayGeneric(const std::string& file, Solver* solver, Tp* na, Tp* ca, const std::string& varname, 
                              pvo_var_type_t type, pvo_cookie_t cookie, pvo_vtu_file_t fo, pvo_float3_t* pts,
                              int* cia, int* cja, uint8_t* types)
{
    // Print either node data or cell data depending on which of the two variables are not 0.
    // You shouldn't pass two non-null pointers to the functions because the cell array would
    // overwrite the node array
    if(na)
        pvo_cookie_insert_var(cookie, PVO_VAR_NODEDATA, type, 1, varname.c_str(), na);
    if(ca)
        pvo_cookie_insert_var(cookie, PVO_VAR_CELLDATA, type, 1, varname.c_str(), ca);

    pvo_vtu_file_open(file.c_str(), cookie,
                      solver->GetMesh()->nnodes()   , pts,
                      solver->GetMesh()->nelements(), cia, cja, types, &fo);
    pvo_file_write((pvo_file_t )fo);
    pvo_vtu_file_close(fo);

    pvo_cookie_remove_var(cookie, varname.c_str());
}

void PvoWriter::WriteNodalArrayInt(const std::string& file, Solver* solver, int* a, const std::string& varname)
{
    WriteArrayGeneric(file, solver, a, (int* )0, varname, PVO_VAR_INT32, cookie, fo, pts, cia, cja, types);
}

void PvoWriter::WriteNodalArrayFloat(const std::string& file, Solver* solver, float* a, const std::string& varname)
{
    WriteArrayGeneric(file, solver, a, (float* )0, varname, PVO_VAR_FLOAT32, cookie, fo, pts, cia, cja, types);
}

void PvoWriter::WriteNodalArrayDouble(const std::string& file, Solver* solver, double* a, const std::string& varname)
{
    WriteArrayGeneric(file, solver, a, (double* )0, varname, PVO_VAR_FLOAT64, cookie, fo, pts, cia, cja, types);
}

void PvoWriter::WriteElementArrayInt(const std::string& file, Solver* solver, int* a, const std::string& varname)
{
    WriteArrayGeneric(file, solver, (int* )0, a, varname, PVO_VAR_INT32, cookie, fo, pts, cia, cja, types);
}

void PvoWriter::WriteElementArrayFloat(const std::string& file, Solver* solver, float* a, const std::string& varname)
{
    WriteArrayGeneric(file, solver, (float* )0, a, varname, PVO_VAR_FLOAT32, cookie, fo, pts, cia, cja, types);
}

void PvoWriter::WriteElementArrayDouble(const std::string& file, Solver* solver, double* a, const std::string& varname)
{
    WriteArrayGeneric(file, solver, (double* )0, a, varname, PVO_VAR_FLOAT64, cookie, fo, pts, cia, cja, types);
}

