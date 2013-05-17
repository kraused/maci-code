//
// Copyright (c) 2009 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#if 2 == DIM
%module maciS2
#else
%module maciS3
#endif
%{
#include "PluginTable.hh"
%}

/* Get rid of warning 401 */
%include "std_vector.i"
%template(std_vector_Plugin) std::vector<Plugin*>;

%include "PluginTable.hh"

