
// vi: tabstop=4:expandtab

// vi: tabstop=4

#include "md/lammps/src/lammpsPairLJCutWeighted.hh"
#include "md/lammps/src/lammpsPiggyback.hh"

#include <math.h>

#include "memory.h"
/// TODO Clash between Lammps and GA
/// #include "error.h"
#include "atom.h"
#include "comm.h"
#include "neighbor.h"
#include "force.h"
#include "neigh_list.h"

using namespace lammps;


LAMMPS_NS::Pair* lammps::NewPairLJCutWeighted(LAMMPS_NS::LAMMPS *lmp)
{
    return new PairLJCutWeighted(lmp);
}


PairLJCutWeighted::PairLJCutWeighted(LAMMPS_NS::LAMMPS *lmp)
: Pair(lmp)
{
    respa_enable = 0;
}

PairLJCutWeighted::~PairLJCutWeighted()
{
    if(allocated) {
        memory->destroy_2d_int_array(setflag);
        memory->destroy_2d_double_array(cutsq);
        memory->destroy_2d_double_array(cut);
        memory->destroy_2d_double_array(epsilon);
        memory->destroy_2d_double_array(sigma);
        memory->destroy_2d_double_array(lj1);
        memory->destroy_2d_double_array(lj2);
        memory->destroy_2d_double_array(lj3);
        memory->destroy_2d_double_array(lj4);
        memory->destroy_2d_double_array(offset);
    }
}

void PairLJCutWeighted::allocate()
{
    allocated = 1;
    int n = atom->ntypes;

    setflag = memory->create_2d_int_array(n+1,n+1,"pair:setflag");
    for(int i = 1; i <= n; i++) {
        for(int j = i; j <= n; j++) {
            setflag[i][j] = 0;
        }
    }

    cutsq   = memory->create_2d_double_array(n+1,n+1,"pair:cutsq");
    cut     = memory->create_2d_double_array(n+1,n+1,"pair:cut");
    epsilon = memory->create_2d_double_array(n+1,n+1,"pair:epsilon");
    sigma   = memory->create_2d_double_array(n+1,n+1,"pair:sigma");
    lj1     = memory->create_2d_double_array(n+1,n+1,"pair:lj1");
    lj2     = memory->create_2d_double_array(n+1,n+1,"pair:lj2");
    lj3     = memory->create_2d_double_array(n+1,n+1,"pair:lj3");
    lj4     = memory->create_2d_double_array(n+1,n+1,"pair:lj4");
    offset  = memory->create_2d_double_array(n+1,n+1,"pair:offset");
}

void PairLJCutWeighted::settings(int narg, char **arg)
{
    int i, j;

    if(narg != 1)
        abort();
/// TODO Clash
///        error->all("Illegal pair_style command");

    cut_global = force->numeric(arg[0]);

    // reset cutoffs that have been explicitly set

    if(allocated) {
        for(i = 1; i <= atom->ntypes; i++)
            for(j = i+1; j <= atom->ntypes; j++)
                if(setflag[i][j])
                    cut[i][j] = cut_global;
    }
}

void PairLJCutWeighted::coeff(int narg, char **arg)
{
    if(narg < 4 || narg > 5)
        abort();
/// TODO Clash
///        error->all("Incorrect args for pair coefficients");

    if(!allocated)
        allocate();

    int ilo,ihi,jlo,jhi;
    force->bounds(arg[0],atom->ntypes,ilo,ihi);
    force->bounds(arg[1],atom->ntypes,jlo,jhi);

    double epsilon_one = force->numeric(arg[2]);
    double sigma_one = force->numeric(arg[3]);

    double cut_one = cut_global;
    if(narg == 5) cut_one = force->numeric(arg[4]);

    int count = 0;
    for(int i = ilo; i <= ihi; i++) {
        for(int j = std::max(jlo,i); j <= jhi; j++) {
            epsilon[i][j] = epsilon_one;
            sigma[i][j] = sigma_one;
            cut[i][j] = cut_one;
            setflag[i][j] = 1;
            count++;
        }
    }

    if(count == 0) abort();
/// TODO Clash
///    if(count == 0) error->all("Incorrect args for pair coefficients");
}

void PairLJCutWeighted::init_style()
{
    neighbor->request(this);
}

void PairLJCutWeighted::init_list(int id, LAMMPS_NS::NeighList *ptr)
{
    list = ptr;
}

double PairLJCutWeighted::init_one(int i, int j)
{
    double ratio;

    if(setflag[i][j] == 0) {
        epsilon[i][j] = mix_energy(epsilon[i][i], epsilon[j][j], sigma[i][i],sigma[j][j]);
        sigma[i][j]   = mix_distance(sigma[i][i], sigma[j][j]);
        cut[i][j]     = mix_distance(cut[i][i],cut[j][j]);
    }

    lj1[i][j] = 48.0 *epsilon[i][j]*pow(sigma[i][j],12.0);
    lj2[i][j] = 24.0 *epsilon[i][j]*pow(sigma[i][j], 6.0);
    lj3[i][j] = 4.0  *epsilon[i][j]*pow(sigma[i][j],12.0);
    lj4[i][j] = 4.0  *epsilon[i][j]*pow(sigma[i][j], 6.0);

    if(offset_flag) {
        ratio = sigma[i][j] / cut[i][j];
        offset[i][j] = 4.0 * epsilon[i][j] * (pow(ratio,12.0) - pow(ratio,6.0));
    } else {
        offset[i][j] = 0.0;
    }

    lj1[j][i]    = lj1[i][j];
    lj2[j][i]    = lj2[i][j];
    lj3[j][i]    = lj3[i][j];
    lj4[j][i]    = lj4[i][j];
    offset[j][i] = offset[i][j];
    
    // compute I,J contribution to long-range tail correction
    // count total # of atoms of type I and J via Allreduce
    
    if(tail_flag) {
        int *type = atom->type;
        int nlocal = atom->nlocal;
        
        double count[2],all[2];
        count[0] = count[1] = 0.0;
        for(int k = 0; k < nlocal; k++) {
            if(type[k] == i) count[0] += 1.0;
            if(type[k] == j) count[1] += 1.0;
        }
        MPI_Allreduce(count,all,2,MPI_DOUBLE,MPI_SUM,world);
                
        double PI = 4.0*atan(1.0);
        double sig2 = sigma[i][j]*sigma[i][j];
        double sig6 = sig2*sig2*sig2;
        double rc3 = cut[i][j]*cut[i][j]*cut[i][j];
        double rc6 = rc3*rc3;
        double rc9 = rc3*rc6;
        etail_ij = 8.0*PI*all[0]*all[1]*epsilon[i][j] *
            sig6 * (sig6 - 3.0*rc6) / (9.0*rc9); 
        ptail_ij = 16.0*PI*all[0]*all[1]*epsilon[i][j] *
            sig6 * (2.0*sig6 - 3.0*rc6) / (9.0*rc9);
    }
    
    return cut[i][j];
}

double PairLJCutWeighted::single(int i, int j, int itype, int jtype, double rsq, double factor_coul, double factor_lj, double &fforce)
{ 
    double r2inv,r6inv,forcelj,wgti,wgtj;
    
    wgti    = ((lammps::Piggyback* )atom->pb)[i].weight;
    wgtj    = ((lammps::Piggyback* )atom->pb)[j].weight;

    r2inv   = 1.0/rsq;
    r6inv   = r2inv*r2inv*r2inv;
    forcelj = r6inv * (lj1[itype][jtype]*r6inv - lj2[itype][jtype])*0.5*(wgti + wgtj);
    fforce  = factor_lj*forcelj*r2inv;

    return factor_lj*( r6inv*(lj3[itype][jtype]*r6inv-lj4[itype][jtype]) - offset[itype][jtype] );
}

void PairLJCutWeighted::compute(int eflag, int vflag)
{
    int    i,j,ii,jj,inum,jnum,itype,jtype;
    double xtmp,ytmp,ztmp,delx,dely,delz,evdwl,fpair;
    double rsq,r2inv,r6inv,forcelj,factor_lj, wgti, wgtj;
    int    *ilist,*jlist,*numneigh,**firstneigh;

    evdwl = 0.0;
    if(eflag || vflag)
        ev_setup(eflag,vflag);
    else
        evflag = vflag_fdotr = 0;

    double **x = atom->x;
    double **f = atom->f;
    int    *type  = atom->type;
    int    nlocal = atom->nlocal;
    int    nall   = nlocal + atom->nghost;
    double *special_lj = force->special_lj;
    int    newton_pair = force->newton_pair;

    inum       = list->inum;
    ilist      = list->ilist;
    numneigh   = list->numneigh;
    firstneigh = list->firstneigh;

    // loop over neighbors of my atoms

    for(ii = 0; ii < inum; ii++) {
        i = ilist[ii];
        xtmp  = x[i][0];
        ytmp  = x[i][1];
        ztmp  = x[i][2];
        itype = type[i];
        jlist = firstneigh[i];
        jnum  = numneigh[i];
        wgti  = ((lammps::Piggyback* )atom->pb)[i].weight;

        for(jj = 0; jj < jnum; jj++) {
            j = jlist[jj];

            if(j < nall) 
                factor_lj = 1.0;
            else {
                factor_lj = special_lj[j/nall];
                j %= nall;
            }

            delx = xtmp - x[j][0];
            dely = ytmp - x[j][1];
            delz = ztmp - x[j][2];
            rsq = delx*delx + dely*dely + delz*delz;
            jtype = type[j];
            
            if(rsq < cutsq[itype][jtype]) {
                wgtj    = ((lammps::Piggyback* )atom->pb)[j].weight;
                r2inv   = 1.0/rsq;
                r6inv   = r2inv*r2inv*r2inv;
                forcelj = r6inv * (lj1[itype][jtype]*r6inv - lj2[itype][jtype])*0.5*(wgti + wgtj);
                fpair   = factor_lj*forcelj*r2inv;

                f[i][0] += delx*fpair;
                f[i][1] += dely*fpair;
                f[i][2] += delz*fpair; 
                if(newton_pair || j < nlocal) {
                    f[j][0] -= delx*fpair;
                    f[j][1] -= dely*fpair;
                    f[j][2] -= delz*fpair;
                }
                
                if(eflag) {
                    evdwl = wgti*factor_lj*r6inv*(lj3[itype][jtype]*r6inv-lj4[itype][jtype]) - offset[itype][jtype];
                }
        
                if(evflag) 
                    ev_tally(i,j,nlocal,newton_pair,evdwl,0.0,fpair,delx,dely,delz);
            }
        }
    }

    if(vflag_fdotr)
        virial_compute();
}

