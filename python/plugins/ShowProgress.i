//
// Copyright (c) 2010 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#if 2 == DIM
%module pluginsS2
#else
%module pluginsS3
#endif
%{
#include "plugins/ShowProgress.hh"
%}

%include "plugins/ShowProgress.hh"

