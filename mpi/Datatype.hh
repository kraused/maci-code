//
// Copyright (c) 2010 Institute of Computational Science, Universita della Svizzera italiana
//

// vi: tabstop=4:expandtab

#ifndef MACI_MPI_CREATE_DATATYPE_HH_INCLUDED
#define MACI_MPI_CREATE_DATATYPE_HH_INCLUDED 1

#include "mpi/config.hh"

/* automatically created by create-datatype.rb */

/** Create a derived datatype using MPI_Type_create_struct
 *  @param[out] type    the derived datatype to create
 *  @param[in]  Struct  name of the corresponding structure
 *  @param[in]  v01 name of the struct member
 *  @param[in]  type01  MPI datatype corresponding to the member
 *  @param[in]  n01 length of array (1 if no array)
 *  @param[in]  v02 name of the struct member
 *  @param[in]  type02  MPI datatype corresponding to the member
 *  @param[in]  n02 length of array (1 if no array)
 *  @param[in]  v03 name of the struct member
 *  @param[in]  type03  MPI datatype corresponding to the member
 *  @param[in]  n03 length of array (1 if no array)
 */
#define create_struct3(type, Struct, v01, type01, n01,          \
                 v02, type02, n02,          \
                 v03, type03, n03)          \
    do {                                \
        Struct       _mangle03_ex;              \
        int          _mangle03_blens[ 3];           \
        MPI_Aint     _mangle03_displ[ 3],           \
                 _mangle03_addr[ 3];            \
        MPI_Datatype _mangle03_types[ 3];           \
                                    \
        MPI_Get_address(&_mangle03_ex.v01,          \
                &_mangle03_addr[ 0]);           \
        MPI_Get_address(&_mangle03_ex.v02,          \
                &_mangle03_addr[ 1]);           \
        MPI_Get_address(&_mangle03_ex.v03,          \
                &_mangle03_addr[ 2]);           \
                                    \
        _mangle03_displ[ 0] = 0;                \
        _mangle03_displ[ 1] = _mangle03_addr[ 1]        \
                    -_mangle03_addr[0];     \
        _mangle03_displ[ 2] = _mangle03_addr[ 2]        \
                    -_mangle03_addr[0];     \
                                    \
        _mangle03_types[ 0] = type01;               \
        _mangle03_types[ 1] = type02;               \
        _mangle03_types[ 2] = type03;               \
                                    \
        _mangle03_blens[ 0] = n01;              \
        _mangle03_blens[ 1] = n02;              \
        _mangle03_blens[ 2] = n03;              \
                                    \
        MPI_Type_create_struct(3, _mangle03_blens,      \
            _mangle03_displ, _mangle03_types, &(type)); \
                                    \
    } while(0)

/** Create a derived datatype using MPI_Type_create_struct
 *  @param[out] type    the derived datatype to create
 *  @param[in]  Struct  name of the corresponding structure
 *  @param[in]  v01 name of the struct member
 *  @param[in]  type01  MPI datatype corresponding to the member
 *  @param[in]  n01 length of array (1 if no array)
 *  @param[in]  v02 name of the struct member
 *  @param[in]  type02  MPI datatype corresponding to the member
 *  @param[in]  n02 length of array (1 if no array)
 *  @param[in]  v03 name of the struct member
 *  @param[in]  type03  MPI datatype corresponding to the member
 *  @param[in]  n03 length of array (1 if no array)
 *  @param[in]  v04 name of the struct member
 *  @param[in]  type04  MPI datatype corresponding to the member
 *  @param[in]  n04 length of array (1 if no array)
 */
#define create_struct4(type, Struct, v01, type01, n01,          \
                 v02, type02, n02,          \
                 v03, type03, n03,          \
                 v04, type04, n04)          \
    do {                                \
        Struct       _mangle04_ex;              \
        int          _mangle04_blens[ 4];           \
        MPI_Aint     _mangle04_displ[ 4],           \
                 _mangle04_addr[ 4];            \
        MPI_Datatype _mangle04_types[ 4];           \
                                    \
        MPI_Get_address(&_mangle04_ex.v01,          \
                &_mangle04_addr[ 0]);           \
        MPI_Get_address(&_mangle04_ex.v02,          \
                &_mangle04_addr[ 1]);           \
        MPI_Get_address(&_mangle04_ex.v03,          \
                &_mangle04_addr[ 2]);           \
        MPI_Get_address(&_mangle04_ex.v04,          \
                &_mangle04_addr[ 3]);           \
                                    \
        _mangle04_displ[ 0] = 0;                \
        _mangle04_displ[ 1] = _mangle04_addr[ 1]        \
                    -_mangle04_addr[0];     \
        _mangle04_displ[ 2] = _mangle04_addr[ 2]        \
                    -_mangle04_addr[0];     \
        _mangle04_displ[ 3] = _mangle04_addr[ 3]        \
                    -_mangle04_addr[0];     \
                                    \
        _mangle04_types[ 0] = type01;               \
        _mangle04_types[ 1] = type02;               \
        _mangle04_types[ 2] = type03;               \
        _mangle04_types[ 3] = type04;               \
                                    \
        _mangle04_blens[ 0] = n01;              \
        _mangle04_blens[ 1] = n02;              \
        _mangle04_blens[ 2] = n03;              \
        _mangle04_blens[ 3] = n04;              \
                                    \
        MPI_Type_create_struct(4, _mangle04_blens,      \
            _mangle04_displ, _mangle04_types, &(type)); \
                                    \
    } while(0)

#endif

