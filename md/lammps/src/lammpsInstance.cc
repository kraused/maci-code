
// vi: tabstop=4:expandtab

#include "lammpsInstance.hh"
#include "lammpsTimeIntegrator.hh"
#include "lammpsVerletIntegrator.hh"
#include "lammpsRattleIntegrator.hh"
#include "lammpsNoOpIntegrator.hh"

#include "utils/strprintf.hh"

#include "input.h"
#include "atom_vec.h"
#include "fix.h"
#include "comm.h"
#include "domain.h"
#include "modify.h"
#include "pair.h"
#include "force.h"

#include <math.h>
#include <assert.h>


/* Fake main() argc, argv
 */
static int  nargs   = 1 /* 3 */;
static const char *args[] = { "lammps_wo_exe" /*, "-echo", "screen" */ };

/* Callback functions
 */
extern LAMMPS_NS::AtomVec *(*NewAtomVecMultiscaleAtomicCB)(LAMMPS_NS::LAMMPS *, int, char **);
extern LAMMPS_NS::Fix     *(*NewRattleIntegratorCB)(LAMMPS_NS::LAMMPS *, int, char **);
extern LAMMPS_NS::Fix     *(*NewVerletIntegratorCB)(LAMMPS_NS::LAMMPS *, int, char **);
extern LAMMPS_NS::Pair    *(*NewPairLJCutWeightedCB)(LAMMPS_NS::LAMMPS *);
extern LAMMPS_NS::Fix     *(*NewNoOpIntegratorCB)(LAMMPS_NS::LAMMPS *, int, char **);

/* Implementations
 */
namespace lammps 
{
    extern LAMMPS_NS::AtomVec* NewAtomVecMultiscaleAtomic(LAMMPS_NS::LAMMPS *, int, char **);
    extern LAMMPS_NS::Fix*     NewRattleIntegrator(LAMMPS_NS::LAMMPS *, int, char **);
    extern LAMMPS_NS::Fix*     NewVerletIntegrator(LAMMPS_NS::LAMMPS *, int, char **);
    extern LAMMPS_NS::Pair*    NewPairLJCutWeighted(LAMMPS_NS::LAMMPS *);
    extern LAMMPS_NS::Fix*     NewNoOpIntegrator(LAMMPS_NS::LAMMPS *, int, char **);
}


lammps::Instance::Instance(MPI_Comm comm)
: LAMMPS_NS::LAMMPS(nargs, (char** )args, comm), wgt_enabled(false), in_epoch(false), epoch_type(INVALID)
{
    /* Nothing happend so far, so it is okay to set the callback pointers
     * at this point
     */
    NewAtomVecMultiscaleAtomicCB = NewAtomVecMultiscaleAtomic;
    NewRattleIntegratorCB        = NewRattleIntegrator;
    NewVerletIntegratorCB        = NewVerletIntegrator;
    NewPairLJCutWeightedCB       = NewPairLJCutWeighted;
    NewNoOpIntegratorCB          = NewNoOpIntegrator;
}

/** execute for both
 */
static inline void vexecute(LAMMPS_NS::Input *input, const char *fmt, va_list args)
{
    std::string cmd = utils::strvprintf(fmt, args) + "\n";
    input->one(cmd.c_str());
}

void lammps::Instance::execute(const char *fmt, ...)
{
    va_list  args;
    va_start(args, fmt);
    vexecute(input, fmt, args);
    va_end  (args); 
}

void lammps::Instance::execute(const std::string &fmt, ...)
{
    va_list  args;
    va_start(args, fmt);
    vexecute(input, fmt.c_str(), args);
    va_end  (args);
}

void lammps::Instance::setAtomStyle(const char *style)
{
    execute(utils::strprintf("atom_style multiscale_%s", style));
}

MPI_Comm lammps::Instance::createCartWorld()
{
    MPI_Comm cartworld;
    int reorder = 0, periods[] = { 1, 1, 1};

    MPI_Cart_create(this->world, 
            3, 
            this->comm->procgrid,
            periods,
            reorder,
            &cartworld);

    return cartworld;
}

int lammps::Instance::getSpatialRegionIfBox(double *lower, double *upper) const
{
    lower[0] = this->domain->sublo[0];
    lower[1] = this->domain->sublo[1];
    lower[2] = this->domain->sublo[2];

    upper[0] = this->domain->subhi[0];
    upper[1] = this->domain->subhi[1];
    upper[2] = this->domain->subhi[2];

    return 1;
}

void lammps::Instance::read(const char *filename)
{
    execute(utils::strprintf("read_data %s", filename));
}

void lammps::Instance::installVerletIntegrator(::TimeIntegrator *base)
{
    int i;

    execute("fix 1 all multiscale_VerletIntegrator");

    for(i = 0; i < modify->nfix; ++i)
        if(modify->fix[i]->time_integrate)
            dynamic_cast<lammps::VerletIntegrator*>(modify->fix[i])->setBase(base);
}

void lammps::Instance::installRattleIntegrator(::TimeIntegrator *base)
{
    int i;

    execute("fix 1 all multiscale_RattleIntegrator");

    for(i = 0; i < modify->nfix; ++i)
        if(modify->fix[i]->time_integrate)
            dynamic_cast<lammps::RattleIntegrator*>(modify->fix[i])->setBase(base);
}


void lammps::Instance::run_no_op()
{
    execute("fix 1 all multiscale_NoOpIntegrator");
    execute("run 1");
    execute("unfix 1");
}


void lammps::Instance::run(int n)
{
    int i;

    for(i = 0; i < modify->nfix; ++i)
        if(modify->fix[i]->time_integrate)
            dynamic_cast<lammps::TimeIntegrator*>(modify->fix[i])->pre_integration();

    execute(utils::strprintf("run %d", n));

    for(i = 0; i < modify->nfix; ++i)
        if(modify->fix[i]->time_integrate)
            dynamic_cast<lammps::TimeIntegrator*>(modify->fix[i])->post_integration();
}


void lammps::Instance::centerOfMass(double x[]) const
{
    double M = 0.0;

    std::fill(x, x+3, 0.0);
    for(int i = 0; i < this->atom->nlocal; ++i) {
        x[0] += this->atom->mass[this->atom->type[i]]*this->atom->x[i][0];
        x[1] += this->atom->mass[this->atom->type[i]]*this->atom->x[i][1];
        x[2] += this->atom->mass[this->atom->type[i]]*this->atom->x[i][2];

        M    += this->atom->mass[this->atom->type[i]];
    }

    MPI_Allreduce(MPI_IN_PLACE, &M, 1, MPI_DOUBLE, MPI_SUM, this->world);
    MPI_Allreduce(MPI_IN_PLACE,  x, 3, MPI_DOUBLE, MPI_SUM, this->world);

    x[0] /= M;
    x[1] /= M;
    x[2] /= M;
}

void lammps::Instance::createInitialWave(const double midpt[])
{
    double A     = 0.15,
           sigma = 15.0,
           b     = 0.1,
           Lc    = 5*sigma,
           uc    = exp(-Lc*Lc/2)/(sigma*sigma),
           H     = 0.25*sigma, r, v;
    double x[3];

    for(int i = 0; i < this->atom->nlocal; ++i) 
    {
        x[0] = this->atom->x[i][0] - midpt[0];
        x[1] = this->atom->x[i][1] - midpt[1];
        x[2] = this->atom->x[i][2] - midpt[2];
        r    = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
        v    = A*(A*exp(-r*r/(sigma*sigma)))/(A - uc)*(1 + b*cos(2*M_PI*r/H));

        if(r >= 1e-12) 
        {
            this->atom->x[i][0] += v*x[0]/r;
            this->atom->x[i][1] += v*x[1]/r;
            this->atom->x[i][2] += v*x[2]/r;
        }
    }
}

void lammps::Instance::Set_Pair_Style_LJ(double rcut)
{
    execute(utils::strprintf("pair_style lj/cut %f", rcut));
}

void lammps::Instance::Set_Pair_Style_LJ_Weighted(double rcut)
{
    execute(utils::strprintf("pair_style multiscale_ljw/cut %f", rcut));
    wgt_enabled = true;
}


void lammps::Instance::AccessBegin(int type)
{
    int i;
    int N = this->atom->nlocal;

    ASSERT(not in_epoch and INVALID == epoch_type);

    in_epoch   = true;
    epoch_type = type;

    if(epoch_type & (RD_FORCE | WR_FORCE))
    {
        if(1.0 != force->ftm2v)
            for(i = 0; i < N; ++i)
            {
                this->atom->f[i][0] *= force->ftm2v;
                this->atom->f[i][1] *= force->ftm2v;
                this->atom->f[i][2] *= force->ftm2v;
            }
    }
}

void lammps::Instance::AccessEnd()
{
    int i;
    int N = this->atom->nlocal;

    if(epoch_type & (RD_FORCE | WR_FORCE))
    {
        if(1.0 != force->ftm2v)
            for(i = 0; i < N; ++i)
            {
                this->atom->f[i][0] /= force->ftm2v;
                this->atom->f[i][1] /= force->ftm2v;
                this->atom->f[i][2] /= force->ftm2v;
            }
    }

    in_epoch   = false;
    epoch_type = INVALID;
}

