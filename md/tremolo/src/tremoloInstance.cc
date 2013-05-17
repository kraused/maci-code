
// vi: tabstop=4:expandtab

#include "md/tremolo/src/tremoloInstance.hh"

/// TODO config file
#define HAVE_SYS_TYPES_H
#define HAVE_DIRENT_H

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif


/// Global variable which is read by the force calculation methods in Tremolo
/// (we cannot easily pass this as an argument)
int tremolo_wgt_enabled = 0;


static const tremolo::abstract_status_t stats[tremolo::INSTANCE_STATS_LENGTH] = 
{
    tremolo::ACTIVE,
    tremolo::ACTIVE,
    tremolo::INACTIVE,  /* No visualization */
    tremolo::INACTIVE,  /* No measurement */
    tremolo::INACTIVE   /* No checkpointing */
};


tremolo::Instance::Instance(MPI_Comm comm)
: instance_t(comm, stats), nx(0), fx(0), cellrcut(0.0), in_epoch(false), epoch_type(INVALID), wgt_enabled(false)
{
    procs[0] = 0;
    procs[1] = 0;
    procs[2] = 0;

    int numprocs;
    MPI_Comm_size(comm, &numprocs);

    // Warning: If the simulation is two dimensional (or has a very narrow
    //          z extent) and numprocs is very large this will bring problems
    //          because Tremolo cannot use more processes in a given direction
    //          than about a third of the number of cells in the direction.
    MPI_Dims_create(numprocs, 3, procs);
}

tremolo::Instance::~Instance()
{
    if(fx) {
        delete[] fx;
    }

    tremolo::instance_exit(this);
}

void tremolo::Instance::SetProcessors(const int processors[3])
{
    procs[0] = processors[0];
    procs[1] = processors[1];
    procs[2] = processors[2];

    int numprocs;
    MPI_Comm_size(comm, &numprocs);
    
    if(numprocs != procs[0]*procs[1]*procs[2]) {
        fprintf(stderr, " error: SetProcessors((int[3]) { %d, %d, %d })."
                " Input invalid.\n", procs[0], procs[1], procs[2]);
        MPI_Abort(MPI_COMM_WORLD, 128);
    }
}

void tremolo::Instance::SetSimulationBox(const double box[2][3])
{
    if(0.0 != box[0][0] || 0.0 != box[0][1] || 0.0 != box[0][2]) {
        fprintf(stderr, " warn: Tremolo only supports simluation boxes"
                " with left lower corner ( 0.0, 0.0, 0.0 ).\n");
    }

    corner[0] = box[1][0];
    corner[1] = box[1][1];
    corner[2] = box[1][2];
}

void tremolo::Instance::SetExtforces(int n, const double F[][3])
{
    nx = n;
    fx = (double (*)[3])new double[3*n];
    for(int i = 0; i < n; ++i) {
        if(F[i]) {
            fx[i][0] = F[i][0];
            fx[i][1] = F[i][1];
            fx[i][2] = F[i][2];
        }
    }
}

void tremolo::Instance::CenterOfMass(double X[3]) const
{
    X[0] = 0.0;
    X[1] = 0.0;
    X[2] = 0.0;

    double M = 0.0;

    const Particle *p;
    particles_for_each(this, p) 
    {
        trans2(p->x);
        
        X[0] += p->ParDat->mass*p->x[0];
        X[1] += p->ParDat->mass*p->x[1];
        X[2] += p->ParDat->mass*p->x[2];
        
        transF(p->x);

        M += p->ParDat->mass;
    }
        
    MPI_Allreduce(MPI_IN_PLACE, &M, 1, MPI_DOUBLE, MPI_SUM, comm);
    MPI_Allreduce(MPI_IN_PLACE,  X, 3, MPI_DOUBLE, MPI_SUM, comm);
    
    X[0] /= M;
    X[1] /= M;
    X[2] /= M;
}

long tremolo::Instance::NlocalAtoms() const
{
    long  nlocal = 0;
    const Particle* q;

    particles_for_each(this, q)
    {
        ++nlocal;
    }

    return nlocal;
}

static void WriteParameters(const std::string& proj, double box[3], double cellrcut)
{
    std::string filename;
    FILE *fh;

    // Write .parameters file
    filename = proj + ".parameters";
    
    if(NULL == (fh = fopen(filename.c_str(), "w"))) {
        fprintf(stderr, " error: %s failed at %s:%d.\n", __FUNCTION__,
                __FILE__, __LINE__);
        MPI_Abort(MPI_COMM_WORLD, 128);
    }
        
    fprintf(fh,
        "integration: type=dynamics;\n"
        "domain\n"
        "{\n"
        "size: type=diag, length_x=%16.15f, length_y=%16.15f, length_z=%16.15f;\n"
        "};\n"
        "dynamics\n"
        "{\n"
        "propagator, verlet: delta_T=%16.15f, endtime=0.;\n"
        "thermostat\n"
        "{\n"
        "timeline: state=off, [time,temperature,interpolation=(0.,0.,constant)];\n"
        "};\n"
        "};\n"
        "lcs: cellrcut=%16.15f;\n",
        box[0], box[1], box[2], 1.0, cellrcut);

    fclose(fh);
}

static void WritePotentials(const std::string& proj, double sigma, double eps, double mass, double rcut)
{
    std::string filename;
    FILE *fh;

    // Write .potentials file
    filename = proj + ".potentials";
    
    if(NULL == (fh = fopen(filename.c_str(), "w"))) {
        fprintf(stderr, " error: %s failed at %s:%d.\n", __FUNCTION__,
                __FILE__, __LINE__);
        MPI_Abort(MPI_COMM_WORLD, 128);
    }
        
    fprintf(fh,
        "particles\n"
        "{\n"
        "particle: particle_type=P, sigma=%16.15f, epsilon=%16.15f,\n"
        "sigma14=1.000000e+00, epsilon14=1.000000e+00,\n"
        "mass=%16.15f,  free=3., charge=0.000000e+00;\n"
        "};\n"
        "nonbonded_2body_potentials\n"
        "{\n"
        "lennardjones: particle_type1=P, particle_type2=P, r_cut=%16.15f;\n"
        "};", sigma, eps, mass, rcut);

    fclose(fh);
}

static void WriteValidates(const std::string& proj)
{
    std::string filename;
    FILE *fh;

    // Write .validates file
    filename = proj + ".validates";
    
    if(NULL == (fh = fopen(filename.c_str(), "w"))) {
        fprintf(stderr, " error: %s failed at %s:%d.\n", __FUNCTION__,
                __FILE__, __LINE__);
        MPI_Abort(MPI_COMM_WORLD, 128);
    }
        
    fprintf(fh, "validate: particle_type=P, state=on;\n");

    fclose(fh);
}

static std::string ExtractProjname(std::string filename)
{
    std::size_t i = filename.find_last_of("/");

    /// The string t contains the basename of the file
    std::string t;
    if(std::string::npos == i)
        t = filename;
    else
        t = filename.substr(i+1);

    /// Chop of the suffix. Typically the suffix will be .data
    return t.substr(0, t.find_last_of("."));
}

static std::string CurrentWDir()
{
    char buffer[1024];

    getcwd(buffer, 128);
    return std::string(buffer);
}

#undef  COPY_BUF_SPACE
#define COPY_BUF_SPACE  (1024*1024)

/// If filename is not in the current working directory of the code,
/// copy it to this directory using 1 MB chunks of memory. 
///
/// Note that we cannot simply create a symbolic link because we typically
/// run the code on a scratch filesystem which might differ from the
/// filesystem containing the input file itself.
static void CopyDatafile(const std::string& proj, std::string filename)
{
    std::size_t i = filename.find_last_of("/");

    /// Both strings do not contain a trailing "/"
    std::string dirname  = filename.substr(0, i);
    std::string basename = filename.substr(i+1);
    std::string cwd      = CurrentWDir();

    if(dirname != cwd)
    {
        std::string outfile = cwd + "/" + basename;

        printf(" [ 0]: Copying \"%s\" to \"%s\" in chunks of 1 MB.\n", filename.c_str(), outfile.c_str());

        FILE* fi  = fopen(filename.c_str(), "r");
        FILE* fo  = fopen(outfile.c_str(), "w");

        char* buf = new char[COPY_BUF_SPACE];

        while(not feof(fi))
        {
            i = fread(buf, 1, COPY_BUF_SPACE, fi);
            fwrite(buf, 1, i, fo);
        }

        delete[] buf;

        fclose(fi);
        fclose(fo);
    }
}

/// There is a problem on NFS mounted drives that fopen will fail
/// for the file written by CopyDatafile because there is outdated cached
/// meta-data information. We need to open and close the directory
/// to update the meta-data information.
static void NFS_fopen_Workaround(const std::string& proj, std::string filename)
{
    std::size_t i = filename.find_last_of("/");
        
    /// Both strings do not contain a trailing "/"
    std::string dirname  = filename.substr(0, i);
    std::string basename = filename.substr(i+1);
    std::string cwd      = CurrentWDir();

    if(dirname == cwd)
        return;

    bool found = true;

    DIR* dir = opendir(cwd.c_str());
    if(NULL == dir)
    {
        fprintf(stderr, " error: opendir() failed.\n");
        MPI_Abort(MPI_COMM_WORLD, 128);
    }

    for(dirent* dp = readdir(dir); dp; dp = readdir(dir))
    {
        if(basename == dp->d_name)
            found = true;
    }
    closedir(dir);

    if(!found)
    {
        fprintf(stderr, " error: could not find \"%s\" in the current directory.\n", cwd.c_str());
        MPI_Abort(MPI_COMM_WORLD, 128);
    }

}

void tremolo::Instance::Init(const char *filename)
{
    std::string proj;

    int myrank;
    MPI_Comm_rank(comm, &myrank);

    if(0 == myrank) 
    {
        proj = ExtractProjname(filename);

        /// We need to make sure that the input file is in
        /// the working directory of the code.
        CopyDatafile(proj, filename);
        
        WriteParameters(proj, corner, cellrcut);
        WritePotentials(proj, ljsigma, ljeps, 1.0, rcut);
        WriteValidates (proj);
    }
    MPI_Barrier(comm);

    /// Work around the fopen problem on NFS mounted devieces
    NFS_fopen_Workaround(proj, filename);

    /// The parameter_file path must be absolute because the Tremolo interface
    /// uses this to identify the input directory.
    std::string parafile = CurrentWDir() + "/" + proj + ".parameters";
    if(parafile.length() >= sizeof(parameter_file)+1) {
        fprintf(stderr, " error: %s failed at %s:%d.\n", __FUNCTION__, __FILE__, __LINE__);
        MPI_Abort(MPI_COMM_WORLD, 128);
    } else {
        strcpy(parameter_file, parafile.c_str());
    }

    this->exts_instance.num = nx;

    instance_init (this);
    instance_gauge(this);

    for(int i = 0; i < nx; ++i) {
        this->exts_instance.force(i)[0] = fx[i][0];
        this->exts_instance.force(i)[1] = fx[i][1];
        this->exts_instance.force(i)[2] = fx[i][2];
    }

    /* Perform initial load balancing */
    instance_loadbal(this);

    loadbal_options.mstep  = 100000;
}

#define NEED_SCALING(t)  ( (t) & (RD_POS  | WR_POS   | RD_VEL | WR_VEL | RD_REFP | WR_REFP | RD_FORCE | WR_FORCE) )

void tremolo::Instance::AccessBegin(int type)
{
    Particle* p;

    ASSERT(not in_epoch and INVALID == epoch_type);

    in_epoch   = true;
    epoch_type = type;
    
    if(NEED_SCALING(epoch_type)) 
        particles_for_each(this,p) 
        {
            trans2(Base::x0(p));
            trans2(p->x);
            scale2(p->u);
            scale2(p->F);
        }
}

void tremolo::Instance::AccessEnd()
{
    Particle* p;

    if(NEED_SCALING(epoch_type)) 
        particles_for_each(this,p) 
        {
            transF(Base::x0(p));
            transF(p->x);
            scaleF(p->u);
            scaleF(p->F);
        }   
    
    if(epoch_type & WR_POS)
        tremolo::instance_fix(this);

    in_epoch   = false;
    epoch_type = INVALID;
}

void tremolo::Instance::Enable_wgt()
{
    printf(" Enabling weighting in the time integration.");
    wgt_enabled = true; tremolo_wgt_enabled = 1;
}

