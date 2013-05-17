
// vi: tabstop=4:expandtab

#ifndef MACI_DELETE_HH_INCLUDED
#define MACI_DELETE_HH_INCLUDED 1

/// Delete a pointer allocated with new
/// @param[in]  p   the pointer
template<typename Tp>
void DeletePointer(Tp* p)
{
    if(p)
        delete p;
}

/// Delete an array allocated with new[]
/// @param[in]  p   the array
template<typename Tp>
void DeleteArray(Tp* p)
{
    if(p)
        delete[] p;
}

#endif

