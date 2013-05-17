
// vi: tabstop=4:expandtab

#include "lammpsPiggyback.hh"

int lammps::packPiggyback(lammps::Piggyback *pb, double *buf)
{
    int m = 0;

    buf[m++] = (double )pb->source;
    buf[m++] = (double )pb->bdIndex;
    for(int i = 0; i < Piggyback::MAX_FEPROCS; ++i) {
        buf[m++] = (double )pb->feprocs[i];
    }
    for(int i = 0; i < Piggyback::MAX_FEPROCS; ++i) {
        buf[m++] = (double )pb->fedispl[i];
    }
    buf[m++] = (double )pb->origin;
    buf[m++] = (double )pb->locIndex;
    for(int i = 0; i < Piggyback::MAX_BDPROCS; ++i) {
        buf[m++] = pb->bdprocs[i];
    }
    for(int i = 0; i < Piggyback::MAX_BDPROCS; ++i) {
        buf[m++] = (double )pb->bddispl[i];
    }
    buf[m++] = pb->weight;
    buf[m++] = pb->dampingD;

    return m;
}

int lammps::unpackPiggyback(double *buf, lammps::Piggyback *pb)
{
    int m = 0;

    pb->source  = (int )buf[m++];
    pb->bdIndex = (int )buf[m++];
    for(int i = 0; i < Piggyback::MAX_FEPROCS; ++i) {
        pb->feprocs[i] = (int )buf[m++];
    }
    for(int i = 0; i < Piggyback::MAX_FEPROCS; ++i) {
        pb->fedispl[i] = (int )buf[m++];
    }
    pb->origin   = (int )buf[m++];
    pb->locIndex = (int )buf[m++];
    for(int i = 0; i < Piggyback::MAX_BDPROCS; ++i) {
        pb->bdprocs[i] = (int )buf[m++];
    }
    for(int i = 0; i < Piggyback::MAX_BDPROCS; ++i) {
        pb->bddispl[i] = (int )buf[m++];
    }
    pb->weight   = buf[m++];
    pb->dampingD = buf[m++];

    return m;
}

