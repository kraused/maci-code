
// vi: tabstop=4:expandtab

#ifndef MESHING_MESHING_HH_INCLUDED
#define MESHING_MESHING_HH_INCLUDED 1

#include <Python.h>

// Check if two boxes (defined by their lower and upper corner) have a non-zero
// intersection
bool box_have_intersection(int dimension, PyObject *box1, PyObject *box2);

// Compute the lower and upper corner of the intersection polytope
void box_intersection(int dimension, PyObject *box1, PyObject *box2, PyObject *cut);

// Refine a box
void box_refine(int dimension, PyObject *box, PyObject *subs);

// Compute the volume of a box
double box_volume(int dimension, PyObject *box);

// Uniqify a point list
void point_list_uniqify(PyObject *list_in, PyObject *list_out);

// Creat a point list
void *point_list_create(PyObject *py_list);

// Delete a point list
void point_list_delete(void *list);

// Find the index the of a node in the point list
int point_list_find(void *list, PyObject *p);

#endif

