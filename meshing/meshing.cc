
// vi: tabstop=4:expandtab

#include "meshing/meshing.hh"
#include "polylib/polylib.hh"

#include <vector>

// Read a list of doubles
void read_double_list(PyObject *list, std::size_t sz, double *out)\
{
    if(!PyList_Check(list)) {
        PyErr_SetString(PyExc_TypeError, "not a list");
        abort();
    }

    for(std::size_t i = 0; i < std::min((std::size_t )PyList_Size(list),sz); ++i) {
        out[i] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem(list,i)));
    }
}

// Write to a list of double
void write_double_list(double *in, std::size_t sz, PyObject *out)
{
    if(!PyList_Check(out)) {
        PyErr_SetString(PyExc_TypeError, "not a list");
        abort();
    }

    for(std::size_t i = 0; i < sz; ++i) {
        PyList_Append(out, PyFloat_FromDouble(in[i]));
    }
}

// Version of the box_have_intersection routine which gets the dimension as a
// template argument
template<int D>
bool box_have_intersection_variant(const double *box1_lower, const double *box1_upper,
                       const double *box2_lower, const double *box2_upper)
{
    return polylib::Box<double,D>(box1_lower,box1_upper). \
        hasIntersection(polylib::Box<double,D>(box2_lower, box2_upper), 1e-5);
}

bool box_have_intersection(int dimension, PyObject *box1, PyObject *box2)
{
    double b1[6], b2[6];

    read_double_list(box1, 6, b1);
    read_double_list(box2, 6, b2);

    switch(dimension) {
    case 2:
        return box_have_intersection_variant<2>(b1, b1+3, b2, b2+3);
    case 3:
        return box_have_intersection_variant<3>(b1, b1+3, b2, b2+3);
    default:
        ASSERT(1 == 0);
    }

    return false;
}

// Version of box_intersection which gets the dimension as a template argument
template<int D>
void box_intersection_variant(const double *box1_lower, const double *box1_upper,
                      const double *box2_lower, const double *box2_upper,
                      double *cut_lower, double *cut_upper)
{
    polylib::Box<double,D> cut = polylib::Box<double,D>(box1_lower, box1_upper) \
                    .getIntersection(polylib::Box<double,D>(box2_lower, box2_upper), 1e-5);

    std::copy(cut.getLower(), cut.getLower()+3, cut_lower);
    std::copy(cut.getUpper(), cut.getUpper()+3, cut_upper);
}

void box_intersection(int dimension, PyObject *box1, PyObject *box2, PyObject *cut)
{
    double b1[6], b2[6], c[6];

    read_double_list(box1, 6, b1);
    read_double_list(box2, 6, b2);

    switch(dimension) {
    case 2:
        box_intersection_variant<2>(b1, b1+3, b2, b2+3, c, c+3);
        break;
    case 3:
        box_intersection_variant<3>(b1, b1+3, b2, b2+3, c, c+3);
        break;
    default:
        ASSERT(1 == 0);
    }

    write_double_list(c, 6, cut);
}

// Version of box_refine which gets the dimension as a template argument
template<int D>
void box_refine_variant(const double *box_lower, const double *box_upper,
            double (*subs_lower)[3], double (*subs_upper)[3])
{
    polylib::Box<double,D> subs[(1<<D)];
    polylib::Box<double,D>(box_lower, box_upper).refine(subs);

    for(int i = 0; i < (1<<D); ++i) {
        std::copy(subs[i].getLower(), subs[i].getLower()+3, subs_lower[i]);
        std::copy(subs[i].getUpper(), subs[i].getUpper()+3, subs_upper[i]);
    }
}

void box_refine(int dimension, PyObject *box, PyObject *subs)
{
    double b[6], subs_lower[8][3], subs_upper[8][3];

    read_double_list(box, 6, b);

    switch(dimension) {
    case 2:
        box_refine_variant<2>(b, b+3, subs_lower, subs_upper);
        break;
    case 3:
        box_refine_variant<3>(b, b+3, subs_lower, subs_upper);
        break;
    default:
        ASSERT(1 == 0);
    }

    for(int i = 0; i < (1<<dimension); ++i) {
        write_double_list(subs_lower[i], 3, subs);
        write_double_list(subs_upper[i], 3, subs);
    }
}

// Version of box_volume which gets the dimension as a template argument
template<int D>
double box_volume_variant(const double *box_lower, const double *box_upper)
{
    return polylib::Box<double,D>(box_lower,box_upper).getVolume();
}

double box_volume(int dimension, PyObject *box)
{
    double b[6];

    read_double_list(box, 6, b);

    switch(dimension) {
    case 2:
        return box_volume_variant<2>(b, b+3);
    case 3:
        return box_volume_variant<3>(b, b+3);
    default:
        ASSERT(1 == 0);
    }
    
    return 0.0;
}

// A point in the three-dimensional space
struct Point
{
    double p[3];

    double& operator[](int i)
    {
        return p[i];
    }

    bool operator==(const Point &o) const
    {
        return (fabs(p[0] - o.p[0]) + 
            fabs(p[1] - o.p[1]) + 
            fabs(p[2] - o.p[2])) < 1e-3;
    }

    bool operator <(const Point &o) const
    {
        if((*this) == o) {
            return false;
        }

#define floateq(x,y)    ( fabs(x-y)<1e-3 )

        return (p[0]  < o.p[0]-1e-3)                         ||
               (floateq(p[0], o.p[0]) and p[1]  < o.p[1]-1e-3)           ||
               (floateq(p[0], o.p[0]) and floateq(p[1], o.p[1]) and p[2] < o.p[2]-1e-3);
    }
};

void read_point_list(std::vector<Point> *p, PyObject *list)
{
    if(!PyList_Check(list)) {
        PyErr_SetString(PyExc_TypeError, "not a list");
        abort();
    }

    std::size_t n = PyList_Size(list);
    ASSERT(0 == n%3);

    p->resize(n/3);
    for(std::size_t i = 0; i < n/3; ++i) {
        (*p)[i][0] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem(list,3*i  )));
        (*p)[i][1] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem(list,3*i+1)));
        (*p)[i][2] = PyFloat_AsDouble(PyNumber_Float(PyList_GetItem(list,3*i+2)));
    }
}

void point_list_uniqify(PyObject *list_in, PyObject *list_out)
{
    std::vector<Point> p;
    read_point_list(&p, list_in);

    std::sort(p.begin(), p.end());
    std::size_t n = std::unique(p.begin(), p.end()) - p.begin();

    write_double_list((double* )&p[0], 3*n, list_out);
}

void *point_list_create(PyObject *py_list)
{
    std::vector<Point> *p = new std::vector<Point>();
    read_point_list(p, py_list);

    return  p;
}

void point_list_delete(void *list)
{
    delete (std::vector<Point> *)list;
}

int point_list_find(void *list, PyObject *p)
{
    Point pt;
    read_double_list(p, 3, (double* )&pt);

    std::vector<Point> *lst = (std::vector<Point> *)list;

    std::vector<Point>::iterator i = std::find(lst->begin(), lst->end(), pt);
    if(i == lst->end()) {
        abort();
    }

    return i - lst->begin();
}

