/*
------------------------------------------------------------------------------
Standard definitions and types, Bob Jenkins
------------------------------------------------------------------------------
*/
#ifndef HASHLIB_STANDARD_H_INCLUDED
#define HASHLIB_STANDARD_H_INCLUDED 1

#include <stdio.h>
#include <stddef.h>

typedef  unsigned long long  ub8;
#undef  UB8MAXVAL
#define UB8MAXVAL 0xffffffffffffffffLL
#undef  UB8BITS
#define UB8BITS 64
typedef    signed long long  sb8;
#undef  SB8MAXVAL
#define SB8MAXVAL 0x7fffffffffffffffLL
typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
#undef  UB4MAXVAL
#define UB4MAXVAL 0xffffffff
typedef    signed long  int  sb4;
#undef  UB4BITS
#define UB4BITS 32
#undef  SB4MAXVAL
#define SB4MAXVAL 0x7fffffff
typedef  unsigned short int  ub2;
#undef  UB2MAXVAL
#define UB2MAXVAL 0xffff
#undef  UB2BITS
#define UB2BITS 16
typedef    signed short int  sb2;
#undef  SB2MAXVAL
#define SB2MAXVAL 0x7fff
typedef  unsigned       char ub1;
#undef  UB1MAXVAL
#define UB1MAXVAL 0xff
#undef  UB1BITS
#define UB1BITS 8
typedef    signed       char sb1;   /* signed 1-byte quantities */
#undef  SB1MAXVAL
#define SB1MAXVAL 0x7f
typedef                 int  word;  /* fastest type available */

#undef  bis
#define bis(target,mask)  ((target) |=  (mask))
#undef  bic
#define bic(target,mask)  ((target) &= ~(mask))
#undef  bit
#define bit(target,mask)  ((target) &   (mask))
#undef  bit
#undef  min
#define min(a,b) (((a)<(b)) ? (a) : (b))
#undef  max
#define max(a,b) (((a)<(b)) ? (b) : (a))
#undef  align
#define align(a) (((ub4)a+(sizeof(void *)-1))&(~(sizeof(void *)-1)))
#undef  abs
#define abs(a)   (((a)>0) ? (a) : -(a))
#undef  TRUE
#define TRUE  1
#undef  FALSE
#define FALSE 0
#undef  SUCCESS
#define SUCCESS 0  /* 1 on VAX */

#endif /* HASHLIB_STANDARD_H_INCLUDED */
