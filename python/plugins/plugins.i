//
// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#if 2 == DIM
%module pluginsS2
#else
%module pluginsS3
#endif
%{

/* Need to add this so that SWIG knows about
   the base class 
 */
#include "Pointers.hh"

%}

%import  "python/maci/Pointers.i"
%include "Plugin.i"
%include "Visualize.i"
%include "MonitorResidual.i"
%include "MonitorDampingForce.i"
%include "MeasureEnergy.i"
%include "ShowProgress.i"

