
// vi: tabstop=4:expandtab

#include "md/lammps/src/lammpsAtomVecMultiscaleAtomic.hh"

#include "memory.h"
#include "atom.h"
#include "modify.h"
#include "fix.h"
#include "domain.h"
/// TODO error.h from GA and from Lammps clash
/// #include "error.h"

using namespace lammps;


LAMMPS_NS::AtomVec* lammps::NewAtomVecMultiscaleAtomic(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg)
{
    return new AtomVecMultiscaleAtomic(lmp, narg, arg);
}

AtomVecMultiscaleAtomic::AtomVecMultiscaleAtomic(LAMMPS_NS::LAMMPS *lmp, int narg, char **arg)
: AtomVec(lmp, narg, arg), x0(0), pb(0)
{
    molecular = 0;
    mass_type = 1;

    comm_x_only    = 0;
    comm_f_only    = 1;
    size_forward   = 3 + 1;
    size_reverse   = 3;
    size_border    = 6 + 1;
    size_velocity  = 3;
    size_data_atom = 5;
    size_data_vel  = 4;
    xcol_data      = 3;
}

void AtomVecMultiscaleAtomic::grow(int n)
{
    /* If n == 0 the array size is increased by 10000, otherwise we allocate arrays
       of size n */
    if(0 == n)
    nmax += 10000;
    else
    nmax  = n;
    atom->nmax = nmax;

    tag   = atom->tag   = (int *)memory->srealloc(atom->tag  , nmax*sizeof(int), "atom:tag"  );
    type  = atom->type  = (int *)memory->srealloc(atom->type , nmax*sizeof(int), "atom:type" );
    mask  = atom->mask  = (int *)memory->srealloc(atom->mask , nmax*sizeof(int), "atom:mask" );
    image = atom->image = (int *)memory->srealloc(atom->image, nmax*sizeof(int), "atom:image");
    x     = atom->x     = memory->grow_2d_double_array(atom->x, nmax, 3, "atom:x");
    v     = atom->v     = memory->grow_2d_double_array(atom->v, nmax, 3, "atom:v");
    f     = atom->f     = memory->grow_2d_double_array(atom->f, nmax, 3, "atom:f");

    x0   = memory->grow_2d_double_array (x0, nmax, 3, "x0");
    atom->x0 = x0;
    pb       = (Piggyback *)memory->srealloc(pb, nmax*sizeof(Piggyback), "pb");
    atom->pb = (void *)pb;

    if(atom->nextra_grow)
    for(int iextra = 0; iextra < atom->nextra_grow; iextra++)
        modify->fix[atom->extra_grow[iextra]]->grow_arrays(nmax);
}

void AtomVecMultiscaleAtomic::grow_reset()
{
    tag   = atom->tag;
    type  = atom->type;
    mask  = atom->mask;
    image = atom->image;
    x     = atom->x;
    v     = atom->v;
    f     = atom->f;

    x0    = atom->x0;
    pb    = (Piggyback* )atom->pb;
}

void AtomVecMultiscaleAtomic::copy(int i, int j)
{
    tag[j]   = tag[i];
    type[j]  = type[i];
    mask[j]  = mask[i];
    image[j] = image[i];
    x[j][0]  = x[i][0];
    x[j][1]  = x[i][1];
    x[j][2]  = x[i][2];
    v[j][0]  = v[i][0];
    v[j][1]  = v[i][1];
    v[j][2]  = v[i][2];

    x0[j][0] = x0[i][0];
    x0[j][1] = x0[i][1];
    x0[j][2] = x0[i][2];

    memcpy(pb+j, pb+i, sizeof(Piggyback));

    if(atom->nextra_grow)
    for(int iextra = 0; iextra < atom->nextra_grow; iextra++)
        modify->fix[atom->extra_grow[iextra]]->copy_arrays(i,j);
}

int AtomVecMultiscaleAtomic::pack_comm(int n, int *list, double *buf, int pbc_flag, int *pbc)
{
    int    i, j = 0, m;
    double dx, dy, dz;

    m = 0;
    if(pbc_flag == 0) {
    for(i = 0; i < n; i++) {
        j = list[i];
        buf[m++] = x[j][0];
        buf[m++] = x[j][1];
        buf[m++] = x[j][2];

        buf[m++] = pb[j].weight;
    }
    } else {
    if(domain->triclinic == 0) {
        dx = pbc[0]*domain->xprd;
        dy = pbc[1]*domain->yprd;
        dz = pbc[2]*domain->zprd;
    } else {
        dx = pbc[0]*domain->xprd + pbc[5]*domain->xy + pbc[4]*domain->xz;
        dy = pbc[1]*domain->yprd + pbc[3]*domain->yz;
        dz = pbc[2]*domain->zprd;
    }
    for(i = 0; i < n; i++) {
        j = list[i];
        buf[m++] = x[j][0] + dx;
        buf[m++] = x[j][1] + dy;
        buf[m++] = x[j][2] + dz;

        buf[m++] = pb[j].weight;
    }
    }

    return m;
}

int AtomVecMultiscaleAtomic::pack_comm_vel(int n, int *list, double *buf, int pbc_flag, int *pbc)
{
    int    i, j = 0, m;
    double dx, dy, dz;

    m = 0;
    if(pbc_flag == 0) {
    for(i = 0; i < n; i++) {
        j = list[i];
        buf[m++] = x[j][0];
        buf[m++] = x[j][1];
        buf[m++] = x[j][2];
        buf[m++] = v[j][0];
        buf[m++] = v[j][1];
        buf[m++] = v[j][2];
        
        buf[m++] = pb[j].weight;
    }
    } else {
    if(domain->triclinic == 0) {
        dx = pbc[0]*domain->xprd;
        dy = pbc[1]*domain->yprd;
        dz = pbc[2]*domain->zprd;
    } else {
        dx = pbc[0]*domain->xprd + pbc[5]*domain->xy + pbc[4]*domain->xz;
        dy = pbc[1]*domain->yprd + pbc[3]*domain->yz;
        dz = pbc[2]*domain->zprd;
    }
    for(i = 0; i < n; i++) {
        j = list[i];
        buf[m++] = x[j][0] + dx;
        buf[m++] = x[j][1] + dy;
        buf[m++] = x[j][2] + dz;
            buf[m++] = v[j][0];
            buf[m++] = v[j][1];
            buf[m++] = v[j][2];

            buf[m++] = pb[j].weight;
    }
    }

    return m;
}

void AtomVecMultiscaleAtomic::unpack_comm(int n, int first, double *buf)
{
    int i, m, last;
    
    m = 0;
    last = first + n;
    for(i = first; i < last; i++) {
    x[i][0] = buf[m++];
    x[i][1] = buf[m++];
    x[i][2] = buf[m++];

    pb[i].weight = buf[m++];
    }    
}    
    
void AtomVecMultiscaleAtomic::unpack_comm_vel(int n, int first, double *buf)
{
    int i, m, last;

    m = 0;                           
    last = first + n;
    for(i = first; i < last; i++) {
    x[i][0] = buf[m++];
    x[i][1] = buf[m++];
    x[i][2] = buf[m++];
    v[i][0] = buf[m++];
    v[i][1] = buf[m++];
    v[i][2] = buf[m++];
    
    pb[i].weight = buf[m++];
    }    
}    

int AtomVecMultiscaleAtomic::pack_reverse(int n, int first, double *buf)
{    
    int i, m, last;
        
    m = 0;
    last = first + n;
    for(i = first; i < last; i++) {
    buf[m++] = f[i][0];
    buf[m++] = f[i][1];
    buf[m++] = f[i][2];
    } 

    return m;
}    
        
void AtomVecMultiscaleAtomic::unpack_reverse(int n, int *list, double *buf)
{    
    int i, j, m;
        
    m = 0;
    for(i = 0; i < n; i++) {
    j = list[i]; 
    f[j][0] += buf[m++];
    f[j][1] += buf[m++];
    f[j][2] += buf[m++];
    } 
} 

int AtomVecMultiscaleAtomic::pack_border(int n, int *list, double *buf, int pbc_flag, int *pbc)
{
    int    i, j = 0, m;
    double dx, dy, dz;

    m = 0;
    if(pbc_flag == 0) {
    for(i = 0; i < n; i++) {
        j = list[i];
        buf[m++] = x[j][0];
        buf[m++] = x[j][1];
        buf[m++] = x[j][2];
        buf[m++] = tag[j];
        buf[m++] = type[j];
        buf[m++] = mask[j];
        
        buf[m++] = pb[j].weight;
    }
    } else {
    if(domain->triclinic == 0) {
        dx = pbc[0]*domain->xprd;
        dy = pbc[1]*domain->yprd;
        dz = pbc[2]*domain->zprd;
    } else {
        dx = pbc[0];
        dy = pbc[1];
        dz = pbc[2];
    }
    for(i = 0; i < n; i++) {
        j = list[i];
        buf[m++] = x[j][0] + dx;
        buf[m++] = x[j][1] + dy;
        buf[m++] = x[j][2] + dz;
            buf[m++] = tag[j];
            buf[m++] = type[j];
            buf[m++] = mask[j];
            
        buf[m++] = pb[j].weight;
    }
    }

    return m;
}

int AtomVecMultiscaleAtomic::pack_border_vel(int n, int *list, double *buf, int pbc_flag, int *pbc)
{
    int    i, j = 0, m;
    double dx, dy, dz;

    m = 0;
    if(pbc_flag == 0) {
    for(i = 0; i < n; i++) {
        j = list[i];
        buf[m++] = x[j][0];
        buf[m++] = x[j][1];
        buf[m++] = x[j][2];
        buf[m++] = tag[j];
        buf[m++] = type[j];
        buf[m++] = mask[j];
        buf[m++] = v[j][0];
        buf[m++] = v[j][1];
        buf[m++] = v[j][2];
        
        buf[m++] = pb[j].weight;
    }
    } else {
    if(domain->triclinic == 0) {
        dx = pbc[0]*domain->xprd;
        dy = pbc[1]*domain->yprd;
        dz = pbc[2]*domain->zprd;
    } else {
        dx = pbc[0];
        dy = pbc[1];
        dz = pbc[2];
    }
    for(i = 0; i < n; i++) {
        j = list[i];
        buf[m++] = x[j][0] + dx;
        buf[m++] = x[j][1] + dy;
        buf[m++] = x[j][2] + dz;
            buf[m++] = tag[j];
            buf[m++] = type[j];
            buf[m++] = mask[j];
            buf[m++] = v[j][0];
            buf[m++] = v[j][1];
            buf[m++] = v[j][2];
            
        buf[m++] = pb[j].weight;
    }
    }
    
    return m;
}

void AtomVecMultiscaleAtomic::unpack_border(int n, int first, double *buf)
{
    int i,m,last;

    m = 0;
    last = first + n;
    for(i = first; i < last; i++) {
    if(i == nmax) grow(0);
    x[i][0] = buf[m++];
    x[i][1] = buf[m++];
    x[i][2] = buf[m++];
    tag[i]  = (int )buf[m++];
    type[i] = (int )buf[m++];
    mask[i] = (int )buf[m++];
    
    pb[i].weight = buf[m++];
    }
}

void AtomVecMultiscaleAtomic::unpack_border_vel(int n, int first, double *buf)
{
    int i,m,last;

    m = 0;
    last = first + n;
    for(i = first; i < last; i++) {
    if(i == nmax) grow(0);
    x[i][0] = buf[m++];
    x[i][1] = buf[m++];
    x[i][2] = buf[m++];
    tag[i]  = (int )buf[m++];
    type[i] = (int )buf[m++];
    mask[i] = (int )buf[m++];
    v[i][0] = buf[m++];
    v[i][1] = buf[m++];
    v[i][2] = buf[m++];
    
    pb[i].weight = buf[m++];
    }
}

int AtomVecMultiscaleAtomic::pack_exchange(int i, double *buf)
{
    int m = 1;
    buf[m++] = x[i][0];
    buf[m++] = x[i][1];
    buf[m++] = x[i][2];
    buf[m++] = v[i][0];
    buf[m++] = v[i][1];
    buf[m++] = v[i][2];
    buf[m++] = tag[i];
    buf[m++] = type[i];
    buf[m++] = mask[i];
    buf[m++] = image[i];

    buf[m++] = x0[i][0];
    buf[m++] = x0[i][1];
    buf[m++] = x0[i][2];
    
    m += packPiggyback(pb+i, buf+m);

    if(atom->nextra_grow)
    for(int iextra = 0; iextra < atom->nextra_grow; iextra++)
        m += modify->fix[atom->extra_grow[iextra]]->pack_exchange(i,&buf[m]);

    buf[0] = m;
    return m;
}

int AtomVecMultiscaleAtomic::unpack_exchange(double *buf)
{
    int nlocal = atom->nlocal;
    if(nlocal == nmax) grow(0);

    int m = 1;
    x[nlocal][0] = buf[m++];
    x[nlocal][1] = buf[m++];
    x[nlocal][2] = buf[m++];
    v[nlocal][0] = buf[m++];
    v[nlocal][1] = buf[m++];
    v[nlocal][2] = buf[m++];
    tag[nlocal]   = (int )buf[m++];
    type[nlocal]  = (int )buf[m++];
    mask[nlocal]  = (int )buf[m++];
    image[nlocal] = (int )buf[m++];

    x0[nlocal][0] = buf[m++];
    x0[nlocal][1] = buf[m++];
    x0[nlocal][2] = buf[m++];

    m += unpackPiggyback(buf+m, pb+nlocal);

    if(atom->nextra_grow)
    for(int iextra = 0; iextra < atom->nextra_grow; iextra++)
        m += modify->fix[atom->extra_grow[iextra]]->
        unpack_exchange(nlocal,&buf[m]);

    atom->nlocal++;
    return m;
}

int AtomVecMultiscaleAtomic::size_restart()
{
    int i;

    int nlocal = atom->nlocal;
    int n = 11 * nlocal;

    if(atom->nextra_restart)
    for(int iextra = 0; iextra < atom->nextra_restart; iextra++)
        for(i = 0; i < nlocal; i++)
        n += modify->fix[atom->extra_restart[iextra]]->size_restart(i);

    return n;
}

int AtomVecMultiscaleAtomic::pack_restart(int i, double *buf)
{
    int m = 1;
    buf[m++] = x[i][0];
    buf[m++] = x[i][1];
    buf[m++] = x[i][2];
    buf[m++] = tag[i];
    buf[m++] = type[i];
    buf[m++] = mask[i];
    buf[m++] = image[i];
    buf[m++] = v[i][0];
    buf[m++] = v[i][1];
    buf[m++] = v[i][2];

    buf[m++] = x0[i][0];
    buf[m++] = x0[i][1];
    buf[m++] = x0[i][2];
    
    m += packPiggyback(pb+i, buf+m);

    if(atom->nextra_restart)
    for(int iextra = 0; iextra < atom->nextra_restart; iextra++)
        m += modify->fix[atom->extra_restart[iextra]]->pack_restart(i,&buf[m]);

    buf[0] = m;
    return m;
}

int AtomVecMultiscaleAtomic::unpack_restart(double *buf)
{
    int nlocal = atom->nlocal;
    if(nlocal == nmax) {
    grow(0);
    if(atom->nextra_store)
        atom->extra = memory->grow_2d_double_array(atom->extra,nmax, atom->nextra_store, "atom:extra");
    }

    int m = 1;
    x[nlocal][0]  = buf[m++];
    x[nlocal][1]  = buf[m++];
    x[nlocal][2]  = buf[m++];
    tag[nlocal]   = (int )buf[m++];
    type[nlocal]  = (int )buf[m++];
    mask[nlocal]  = (int )buf[m++];
    image[nlocal] = (int )buf[m++];
    v[nlocal][0]  = buf[m++];
    v[nlocal][1]  = buf[m++];
    v[nlocal][2]  = buf[m++];
    
    x0[nlocal][0] = buf[m++];
    x0[nlocal][1] = buf[m++];
    x0[nlocal][2] = buf[m++];

    m += unpackPiggyback(buf+m, pb+nlocal);
    
    double **extra = atom->extra;
    if(atom->nextra_store) {
    int size = (int )(buf[0]) - m;
    for(int i = 0; i < size; i++) extra[nlocal][i] = buf[m++];
    }

    atom->nlocal++;
    return m;
}

void AtomVecMultiscaleAtomic::create_atom(int itype, double *coord)
{
    int nlocal = atom->nlocal;
    if(nlocal == nmax) grow(0);

    tag[nlocal] = 0;
    type[nlocal] = itype;
    x[nlocal][0] = coord[0];
    x[nlocal][1] = coord[1];
    x[nlocal][2] = coord[2];
    mask[nlocal] = 1;
    image[nlocal] = (512 << 20) | (512 << 10) | 512;
    v[nlocal][0] = 0.0;
    v[nlocal][1] = 0.0;
    v[nlocal][2] = 0.0;

    x0[nlocal][0] = coord[0];
    x0[nlocal][1] = coord[1];
    x0[nlocal][2] = coord[2];

    pb[nlocal].weight = 1.0;

    atom->nlocal++;
}

void AtomVecMultiscaleAtomic::data_atom(double *coord, int imagetmp, char **values)
{
    int nlocal = atom->nlocal;
    if(nlocal == nmax) grow(0);

    tag[nlocal] = atoi(values[0]);
    if(tag[nlocal] <= 0)
        abort();
/// TODO
///    error->one("Invalid atom ID in Atoms section of data file");

    type[nlocal] = atoi(values[1]);
    if(type[nlocal] <= 0 || type[nlocal] > atom->ntypes)
        abort();
/// TODO
///    error->one("Invalid atom type in Atoms section of data file");

    x[nlocal][0] = coord[0];
    x[nlocal][1] = coord[1];
    x[nlocal][2] = coord[2];

    image[nlocal] = imagetmp;

    mask[nlocal] = 1;
    v[nlocal][0] = 0.0;
    v[nlocal][1] = 0.0;
    v[nlocal][2] = 0.0;

    x0[nlocal][0] = coord[0];
    x0[nlocal][1] = coord[1];
    x0[nlocal][2] = coord[2];

    pb[nlocal].weight = 1.0;

    atom->nlocal++;
}

double AtomVecMultiscaleAtomic::memory_usage()
{
    double bytes = 0.0;

    if(atom->memcheck("tag"))   bytes += nmax * sizeof(int);
    if(atom->memcheck("type"))  bytes += nmax * sizeof(int);
    if(atom->memcheck("mask"))  bytes += nmax * sizeof(int);
    if(atom->memcheck("image")) bytes += nmax * sizeof(int);
    if(atom->memcheck("x"))     bytes += nmax*3 * sizeof(double);
    if(atom->memcheck("v"))     bytes += nmax*3 * sizeof(double);
    if(atom->memcheck("f"))     bytes += nmax*3 * sizeof(double);
    
    if(atom->memcheck("x0"))    bytes += nmax*3*sizeof(double);
    if(atom->memcheck("pb"))    bytes += nmax*sizeof(Piggyback);

    return bytes;
}

