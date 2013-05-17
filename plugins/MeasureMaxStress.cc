
// vi: tabstop=4:expandtab

#include "MeasureMaxStress.hh"
#include "ProcElement.hh"
#include "DesignByContract.hh"
#include "exception/Throw.hh"
#include "utils/strprintf.hh"

using namespace plugins;

/* -----------------------------------------------------------------------------
 * Static utility functions
 * -------------------------------------------------------------------------- */
/// Square operation
template<typename Tp>
Tp sqr(Tp x)
{
    return x*x;
}

// /// Compute the norm of the stress at a particle
// static double stressNorm(const md::interface::ParticleHandle &ph)
// {
//  using md::interface::System;
//  using md::interface::ParticleHandle;
//  using md::interface::Property;
//
//  System *sys = gcpe()->GetMMSystem();
//
//  double S = 0.0;
//  for(int i = 0; i < 3; ++i) {
//      for(int j = 0; j < 3; ++j) {
//          CHECK_DEBUG(!isnan(Property::Stress(*sys,ph,i,j)), 0);
//
//          S += sqr(Property::Stress(*sys,ph,i,j));
//      }
//  }
//
//  return sqrt(S);
// }

/// Open the output file for the maximal stresses
static FILE *Open_Max_Stress_Outfile(const std::string &prefix)
{
    FILE *f = 0;

    if(0 == gcpe()->myscale.myrank()) {
        std::string outname = prefix + ".dat";

        f = fopen(outname.c_str(), "w");
        CHECK_ALWAYS(f, 0);
    }

    return f;
}


/* -----------------------------------------------------------------------------
 * Class member functions
 * -------------------------------------------------------------------------- */

MeasureMaxStress::MeasureMaxStress(const char *args)
: Measurement("MeasureMaxStress", args), maxStress(0), maxStressOutfile(0)
{
    if(Plugin::parseString(args, "strategy") == "max"  ) {
        strategy = STRATEGY_MAX;
    } else {
        THROW(BadArgException, "unknown strategy = \"%s\"", Plugin::parseString(args, "strategy").c_str());
    }

    eta      = Plugin::parseDouble(args, "eta");
    treshold = Plugin::parseDouble(args, "treshold");
    prefix   = Plugin::parseString(args, "prefix");

    maxStressOutfile = Open_Max_Stress_Outfile(prefix);
}

MeasureMaxStress::MeasureMaxStress(const MeasureMaxStress &o)
: Measurement(o), strategy(o.strategy), eta(o.eta), treshold(o.treshold),
  maxStress(o.maxStress), prefix(o.prefix), maxStressOutfile(0)
{
    maxStressOutfile = Open_Max_Stress_Outfile(prefix);
}

MeasureMaxStress* MeasureMaxStress::clone() const
{
    return new MeasureMaxStress(*this);
}

void MeasureMaxStress::stop ()
{
    if(0 == gcpe()->myscale.myrank()) {
        fclose(maxStressOutfile);
    }

//  fclose(meshfree09_hack_f);
}

void MeasureMaxStress::apply()
{
    if(doMeasure()) {
//      Measure_Max_Stress(next());
    } else {
        next();
    }
}

//double MeasureMaxStress::computeMaximalStressValue()
//{
//  using md::interface::System;
//  using md::interface::ParticleHandle;
//  using md::interface::Property;
//
//  double S = 0.0;
//
//  System *sys = gcpe()->GetMMSystem();
//  for(ParticleHandle ph = sys->begin(); ph != sys->end(); ++ph) {
//      S = std::max(S, stressNorm(ph));
//  }
//
//  gcpe()->myscale.allreduce(MPI_IN_PLACE, &S, 1, MPI_DOUBLE, MPI_MAX);
//
//  return S;
//}

//std::string MeasureMaxStress::locateLargeStresses(int meshfree09_hack_n)
//{
//  using md::interface::System;
//  using md::interface::ParticleHandle;
//  using md::interface::Property;
//
//  std::string str = "";
//
//  System *sys = gcpe()->GetMMSystem();
//  sys->AccessBegin(System::RD_POS);
//
//  for(ParticleHandle ph = sys->begin(); ph != sys->end(); ++ph) {
//      double S = stressNorm(ph);
//
//      if(S >= eta*maxStress and S >= treshold) {
//          str += utils::strprintf("%d\t%e\t%e\t%e\t%e\n",
//                      meshfree09_hack_n,
//                      Property::Pos(*sys,ph,0), 
//                      Property::Pos(*sys,ph,1), 
//                      Property::Pos(*sys,ph,2), S);
//      }
//  }
//
//  sys->AccessEnd();
//
//  return str;
//}

//void MeasureMaxStress::Measure_Max_Stress(int n)
//{
//  double* S;
//  System* sys = gcpe()->GetMMSystem();
//
//  sys.Locate_Large_Stresses();
//
//  maxStress = computeMaximalStressValue();
//  
//  if(0 == gcpe()->myscale.myrank()) {
//      CHECK_PRECOND_DEBUG(maxStressOutfile);
//      fprintf(maxStressOutfile, "%d\t%e\n", n, maxStress);
//      fflush (maxStressOutfile);
//  }
//
// FIXME
//#if 1 == 0
//  std::string pos = "";
//  // Add the step identifier
//  if(0 == gcpe()->myscale.myrank()) {
//      pos += utils::strprintf("# step %d\n", n);
//  }
//
//  pos += locateLargeStresses();   
//  mpi::CollectiveAsciiWrite(prefix + ".xyz", "a", pos);
//#endif
//  fprintf(meshfree09_hack_f, "%s", locateLargeStresses(n).c_str());
//  
//#ifndef NDEBUG
//  md::interface::System *sys = gcpe()->GetMMSystem();
//  for(md::interface::ParticleHandle ph = sys->begin(); ph != sys->end(); ++ph) {
//      invalidateStress(ph);
//  }
//#endif
//}


