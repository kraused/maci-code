
// vi: tabstop=4:expandtab

#include "MeasureEnergy.hh"
#include "ProcElement.hh"
#include "DesignByContract.hh"
#include "Delete.hh"

using namespace plugins;


/* -----------------------------------------------------------------------------
 * Static utility functions
 * -------------------------------------------------------------------------- */

/// Compute the energy on finite element processing elements
static void measureEnergyFE(fe::interface::Solver& solver, double *K, double *V, double *E)
{
    (*K) = fe::interface::KineticEnergy  (solver);
    (*V) = fe::interface::PotentialEnergy(solver);
    (*E) = (*K) + (*V);
}

/// Compute the energy on MM processing elements
static void measureEnergyMM(double *K, double *V, double *E, ParticleList &list)
{
    (*K) = list.kineticEnergy();
    (*V) = list.potentialEnergy();
    (*E) = (*K) + (*V);
}

// /// Create the particle box
// /// @param[in]  s   the lower and upper corner of the box as comma seperated
// ///         values
// static polylib::Box<double, DIM>* createParticleBox(const std::string &s)
// {
//     double      x[2*3];
//     std::string t;
//     int     k = 0;
// 
//     for(size_t i = 0; i < s.length(); ++i) 
//     {
//         if(' ' == s[i])
//             continue;
//     
//         if(',' == s[i]) 
//         {
//             CHECK_ALWAYS(k < 2*3, 0);
//             x[k++] = atof(t.c_str());
//             t      = "";
//         }
//         else
//             t += s[i];
//     }
// 
//     CHECK_ALWAYS(k == 2*3-1, 0);
//     x[k++] = atof(t.c_str());
// 
//     return new polylib::Box<double, DIM>(x, x+3);
// }

/* -----------------------------------------------------------------------------
 * Class member functions
 * -------------------------------------------------------------------------- */

MeasureEnergy::MeasureEnergy(Maci* app, int mask, const char* outfile)
: Plugin(app, "MeasureEnergy"), mask(mask), step(0), outname(outfile), outfile(0), box(0)
{
    // TODO
    pselect = PARTICLES_SELECT_ALL;

//  if(PROC_MD == this->procel->mytype) 
//  {
//      if(Plugin::parseString(args, "particles") == "all"  ) {
//          pselect = PARTICLES_SELECT_ALL;
//      } else 
//      if(Plugin::parseString(args, "particles") == "nonhs") {
//          pselect = PARTICLES_SELECT_NONHS;
//      } else
//      if(Plugin::parseString(args, "particles") == "box") {
//          pselect = PARTICLES_SELECT_BOX;
//      } else {
//          THROW(BadArgException, "unknown arguments particles = \"%s\"", 
//              Plugin::parseString(args, "particles").c_str());
//      }
//  }

}

MeasureEnergy::MeasureEnergy(const MeasureEnergy &o)
: Plugin(o), mask(o.mask), step(o.step), pselect(o.pselect), outname(o.outname), outfile(0), box(0)
{
    if(o.box)
        box = new polylib::Box<double, DIM>(*o.box);

    if(0 == this->procel->myscale.myrank()) 
        outfile = fopen(outname.c_str(), "a");
}

MeasureEnergy* MeasureEnergy::clone() const
{
    return new MeasureEnergy(*this);
}

MeasureEnergy::~MeasureEnergy()
{
    DeletePointer(box);
}

void MeasureEnergy::setup()
{
//  if(PARTICLES_SELECT_BOX == pselect)
//      box = createParticleBox(Plugin::parseString(args, "box"));

    if(0 == this->procel->myscale.myrank())
    {
        this->outfile = fopen(outname.c_str(), "w");
        CHECK_ALWAYS(outfile, 0);
    }

    // Calculate the reference energy
    measureEnergy(-1);
}

void MeasureEnergy::post_integration()
{
    if(outfile)
        fclose(outfile);
}

void MeasureEnergy::post_step()
{
    ++step;
    if(0 == step % mask)
        measureEnergy(step);
}

ParticleList MeasureEnergy::createParticleList()
{
    switch(pselect) 
    {
        case PARTICLES_SELECT_ALL:
            return ParticleList(SelectAllParticles());
            break;
        case PARTICLES_SELECT_NONHS:
            return ParticleList(SelectNonHandshakeParticles());
            break;
        case PARTICLES_SELECT_BOX:
            CHECK_DEBUG(box, 0);
            return ParticleList(SelectParticlesInBox(*box));
        default:
            THROW(BadDefaultException, "");
    }

    return ParticleList();
}

void MeasureEnergy::measureEnergy(int n)
{
    double       K, V, E;
    ParticleList p;

    switch(this->procel->mytype) 
    {
        case PROC_MD:
            p = MeasureEnergy::createParticleList();
            measureEnergyMM(&K, &V, &E, p);
            break;
        case PROC_FE:
            measureEnergyFE(*this->solver, &K, &V, &E);
            break;
        default:
            THROW(BadDefaultException, "");
    }   

    if(0 == this->procel->myscale.myrank()) 
    {
        CHECK_PRECOND_DEBUG(outfile);
        fprintf(outfile, "%d\t%e\t%e\t%e\n", n, K, V, E);
        fflush (outfile);
    }
}

