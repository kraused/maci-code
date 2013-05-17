
// vi: tabstop=4:expandtab

#include "ElementShadow.hh"
#include "ProcElement.hh"
#include "mpi/Datatype.hh"


/// Construct a datatype for the exchange
static void ConstructDatatype(MPI_Datatype *type)
{   
    create_struct4(*type, ElementShadow,
                   ncorners  , MPI_INT   , 1,
                   corner    , MPI_DOUBLE, 3*ElementShadow::MAX_CORNERS_OF_ELEMENT,
                   owner     , MPI_INT   , 1,
                   localIndex, MPI_INT   , 1);
    MPI_Type_commit(type);
}

MPI_Datatype ElementShadow::type()
{
    static MPI_Datatype t = MPI_DATATYPE_NULL;

    if(MPI_DATATYPE_NULL == t)
        ConstructDatatype(&t);

    return t;
}

polylib::Box<double,DIM> ElementShadow::boundbox() const
{
    double lower[3], upper[3];
    int    i, k;

    for(i = 0; i < DIM; ++i)
        lower[i] = upper[i] = corner[0][i];

    for(k = 0; k < ncorners; ++k)
        for(i = 0; i < DIM; ++i)
        {
            lower[i] = std::min(lower[i], corner[k][i]);
            upper[i] = std::max(upper[i], corner[k][i]);
        }

    return polylib::Box<double,DIM>(lower, upper);
}


void InitializeElementShadow(const fe::interface::Element& el, int localIndex, ElementShadow* elsh)
{
    int i;

    elsh->localIndex = localIndex;
    elsh->owner      = gcpe()->world.myrank();
    elsh->ncorners   = el.ncorners();

    for(i = 0; i < el.ncorners(); ++i)
    {
        std::copy(el.corner(i).coords(), el.corner(i).coords() + DIM, elsh->corner[i]);
#if 2 == DIM
        elsh->corner[i][2] = 0.0;
#endif
    }
}

