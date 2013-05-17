
// vi: tabstop=4:expandtab

// Static variables necessary for the communication with the gather
// and scatter functions
static NodalVector<DIM>* g_t;
static const NodalVector<DIM>* g_map;
static int g_here;


/// ParallelCopyTraits: Traits class for use in ParallelCopy_gather
///          and ParallelCopy_scatter
template<typename Tp>
struct ParallelCopyTraits
{

    /// Invalidate the value of the memory slot p points to.
    /// Here, by "invalid" we mean a state which normally doesn't
    /// occur and which can be checked for
    static void invalidate(Tp* p)
    {
        fprintf(stderr, "ParallelCopyTraits::invalidate is not implemented for this type.");
        MPI_Abort(MPI_COMM_WORLD, __LINE__);
    }

    /// Check if p is invalid
    static bool isInvalid(Tp p)
    {
        fprintf(stderr, "ParallelCopyTraits::isInvalid is not implemented for this type.");
        MPI_Abort(MPI_COMM_WORLD, __LINE__);
        return true;
    }

};

template<>
struct ParallelCopyTraits<double>
{

    static void invalidate(double* p)
    {
        memset(p, 0xff, sizeof(p));
    }

    static bool isInvalid(double p)
    {
        return isnan(p);
    }

};

template<>
struct ParallelCopyTraits<int>
{

    static void invalidate(int* p)
    {
        *p = INT_MAX;
    }

    static bool isInvalid(int p)
    {
        return (INT_MAX == p);
    }

};

template<>
struct ParallelCopyTraits<int64_t>
{

    static void invalidate(int64_t* p)
    {
        *p = INT64_MAX;
    }

    static bool isInvalid(int64_t p)
    {
        return (INT64_MAX == p);
    }

};


/* --------------------------------------------------------------------------------
 * Implementation of SolverImplementation::ParallelCopy
 * -------------------------------------------------------------------------------- */
template<typename Tp, int N>
int ParallelCopy_gather (BorderExchange<DIM>::MeshObject obj, void* data)
{
    if(not BorderExchange<DIM>::isNode(obj))
        return 0;

    NodePointer<DIM> nd = BorderExchange<DIM>::node(obj);
    int i;

    // Warning: While (*g_map)[nd][0] is a double value, it is wrong to
    //          simply cast it to int because we store the integer value
    //          in the memory as integer bit pattern.
    if(g_here == *(int* )(*g_map)[nd])
        for(i = 0; i < N; ++i)
            ((Tp* )data)[i] = ((Tp* )(*g_t)[nd])[i];
    else
        for(i = 0; i < N; ++i)
            ParallelCopyTraits<Tp>::invalidate(&((Tp* )data)[i]);

    return 0;
}

template<typename Tp, int N>
int ParallelCopy_scatter(BorderExchange<DIM>::MeshObject obj, void* data)
{
    if(not BorderExchange<DIM>::isNode(obj))
        return 0;

    NodePointer<DIM> nd = BorderExchange<DIM>::node(obj);
    int i;

    // Skip the owned nodes
    if(g_here == *(int* )(*g_map)[nd])
        return 0;

    for(i = 0; i < N; ++i)
        if(not ParallelCopyTraits<Tp>::isInvalid(((Tp* )data)[i]))
            *(Tp* )&(*g_t)[nd][i] = ((Tp* )data)[i];

    return 0;
}

template<typename Tp, int N>
void UG_IF::SolverImplementation::ParallelCopy(NodalVector* t, const NodalVector* map, int here)
{
    g_t    = t;
    g_map  = map;
    g_here = here;

    BorderExchange<DIM>::Gatherer  f = &ParallelCopy_gather <Tp,N>;
    BorderExchange<DIM>::Scatterer g = &ParallelCopy_scatter<Tp,N>;

    BorderExchange<DIM>::exchange(*mesh, N*sizeof(Tp), f, g);
}

// Since we implement ParallelCopy in the .cc file rather than the header we need to
// explicitly instantiate the function for all types that we will use in the program.
// Note that since MACI uses a Python driver, the DLL is loaded at runtime and linker
// failure will only show up at execution.
template void UG_IF::SolverImplementation::ParallelCopy<double, 1>(NodalVector*, const NodalVector*, int);
template void UG_IF::SolverImplementation::ParallelCopy<double, 2>(NodalVector*, const NodalVector*, int);
template void UG_IF::SolverImplementation::ParallelCopy<double, 3>(NodalVector*, const NodalVector*, int);

template void UG_IF::SolverImplementation::ParallelCopy<int, 1>(NodalVector*, const NodalVector*, int);
template void UG_IF::SolverImplementation::ParallelCopy<int, 2>(NodalVector*, const NodalVector*, int);
template void UG_IF::SolverImplementation::ParallelCopy<int, 3>(NodalVector*, const NodalVector*, int);

template void UG_IF::SolverImplementation::ParallelCopy<int64_t, 1>(NodalVector*, const NodalVector*, int);
template void UG_IF::SolverImplementation::ParallelCopy<int64_t, 2>(NodalVector*, const NodalVector*, int);
template void UG_IF::SolverImplementation::ParallelCopy<int64_t, 3>(NodalVector*, const NodalVector*, int);


/* --------------------------------------------------------------------------------
 * Implementation of SolverImplementation::ParallelSum
 * -------------------------------------------------------------------------------- */
template<typename Tp, int N>
int ParallelSumup_gather (BorderExchange<DIM>::MeshObject obj, void* data)
{
    if(not BorderExchange<DIM>::isNode(obj))
        return 0;

    NodePointer<DIM> nd = BorderExchange<DIM>::node(obj);
    int i;

    for(i = 0; i < N; ++i)
        ((Tp* )data)[i] = *(Tp* )&(*g_t)[nd][i];

    return 0;
}

template<typename Tp, int N>
int ParallelSumup_scatter(BorderExchange<DIM>::MeshObject obj, void* data)
{
    if(not BorderExchange<DIM>::isNode(obj))
        return 0;

    NodePointer<DIM> nd = BorderExchange<DIM>::node(obj);
    int i;

    for(i = 0; i < N; ++i)
        *(Tp* )&(*g_t)[nd][i] += ((Tp* )data)[i];

    return 0;
}

template<typename Tp, int N>
void UG_IF::SolverImplementation::ParallelSumup(NodalVector* t)
{
    g_t    = t;
    g_map  = NULL; // Don't use the map
    g_here = -1;   // Don't use here

    BorderExchange<DIM>::Gatherer  f = &ParallelSumup_gather <Tp,N>;
    BorderExchange<DIM>::Scatterer g = &ParallelSumup_scatter<Tp,N>;

    BorderExchange<DIM>::exchange(*mesh, N*sizeof(Tp), f, g);
}

// Since we implement ParallelSumup in the .cc file rather than the header we need to
// explicitly instantiate the function for all types that we will use in the program.
// Note that since MACI uses a Python driver, the DLL is loaded at runtime and linker
// failure will only show up at execution.
template void UG_IF::SolverImplementation::ParallelSumup<double, 1>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelSumup<double, 2>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelSumup<double, 3>(NodalVector*);

template void UG_IF::SolverImplementation::ParallelSumup<int, 1>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelSumup<int, 2>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelSumup<int, 3>(NodalVector*);

template void UG_IF::SolverImplementation::ParallelSumup<int64_t, 1>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelSumup<int64_t, 2>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelSumup<int64_t, 3>(NodalVector*);


/* --------------------------------------------------------------------------------
 * Implementation of SolverImplementation::ParallelSum
 * -------------------------------------------------------------------------------- */
template<typename Tp, int N>
int ParallelMax_gather (BorderExchange<DIM>::MeshObject obj, void* data)
{
    if(not BorderExchange<DIM>::isNode(obj))
        return 0;

    NodePointer<DIM> nd = BorderExchange<DIM>::node(obj);
    int i;

    for(i = 0; i < N; ++i)
        ((Tp* )data)[i] = *(Tp* )&(*g_t)[nd][i];

    return 0;
}

template<typename Tp, int N>
int ParallelMax_scatter(BorderExchange<DIM>::MeshObject obj, void* data)
{
    if(not BorderExchange<DIM>::isNode(obj))
        return 0;

    NodePointer<DIM> nd = BorderExchange<DIM>::node(obj);
    int i;

    for(i = 0; i < N; ++i)
        *(Tp* )&(*g_t)[nd][i] = std::max(*(Tp* )&(*g_t)[nd][i], ((Tp* )data)[i]);

    return 0;
}

template<typename Tp, int N>
void UG_IF::SolverImplementation::ParallelMax(NodalVector* t)
{
    g_t    = t;
    g_map  = NULL; // Don't use the map
    g_here = -1;   // Don't use here

    BorderExchange<DIM>::Gatherer  f = &ParallelMax_gather <Tp,N>;
    BorderExchange<DIM>::Scatterer g = &ParallelMax_scatter<Tp,N>;

    BorderExchange<DIM>::exchange(*mesh, N*sizeof(Tp), f, g);
}

// Since we implement ParallelMax in the .cc file rather than the header we need to
// explicitly instantiate the function for all types that we will use in the program.
// Note that since MACI uses a Python driver, the DLL is loaded at runtime and linker
// failure will only show up at execution.
template void UG_IF::SolverImplementation::ParallelMax<double, 1>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelMax<double, 2>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelMax<double, 3>(NodalVector*);

template void UG_IF::SolverImplementation::ParallelMax<int, 1>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelMax<int, 2>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelMax<int, 3>(NodalVector*);

template void UG_IF::SolverImplementation::ParallelMax<int64_t, 1>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelMax<int64_t, 2>(NodalVector*);
template void UG_IF::SolverImplementation::ParallelMax<int64_t, 3>(NodalVector*);


