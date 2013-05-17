
#ifndef HASH_HASH_HH_INCLUDED
#define HASH_HASH_HH_INCLUDED 1

extern "C"
{
#include "hash/standard.h"
#include "hash/hashtab.h"
}

#undef min
#undef max
#undef align
#undef abs
#undef TRUE
#undef FALSE
#undef SUCCESS
#undef bis
#undef bic
#undef bit
#undef UB8MAXVAL
#undef UB8MAXVAL
#undef UB8BITS
#undef SB8MAXVAL
#undef UB4MAXVAL
#undef UB4BITS
#undef SB4MAXVAL
#undef UB2MAXVAL
#undef UB2BITS
#undef SB2MAXVAL
#undef UB1MAXVAL
#undef UB1BITS
#undef SB1MAXVAL
#undef STANDARD
#undef STDIO
#undef STDDEF

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash/hashmem.hh"

namespace hash
{

// Hash map implemention
template<typename Key, typename Tp>
class Hashmap
{	

public:
	// Create a new hash map with size two to the power @logsize
	Hashmap(int logsize);
		
	// Delete the instance
	~Hashmap();
	
	// Print statistics about the hash map. Can be useful to optimize
	// performance
	void printStatistics();
	
	// Warning: Be careful whether you pass items by reference or by
	// pointer. It makes a hugh difference!
	
	// Insert a new item with key @key. The hash map does *not* stores the 
	// pointer to @x so the user has to take care that @x is persistent
	// If the key is already in the hashmap the function throws an exception
	// @DUPLICATE_KEY
	void add(const Key& key, Tp* x);
	
	// Insert a new item with key @key. The hash map stores the pointer
	// to a copy of @x so that the user need not bother about the life-
	// time of the item. The function uses @memcpy since it should not rely
	// on a copy constructor
	// If the key is already in the hashmap the function throws an exception
	// @DUPLICATE_KEY
	void add(const Key& key, const Tp& x);
	
	// Returns the item with key @key
	// Throws an exeption @INVALID_KEY if an error occurs.
	Tp* operator()(const Key& key) const;

private:
	// The hash map
	mutable Hashlib_privatescope_htab* tab_;
	// Memory for keys and items
	Memory				   mem_;
	
	// Copy an item or a key in the internal storage. Makes a bitwise copy.
	ub1* copy(const void* x, size_t nmemb);
	
public:
	// Exception: key is invalid
	static const int	INVALID_KEY;
	// Exception: key is already there
	static const int	DUPLICATE_KEY;
};

template<typename Key, typename Tp>
const int       Hashmap<Key,Tp>::INVALID_KEY   = 1;

template<typename Key, typename Tp>
const int       Hashmap<Key,Tp>::DUPLICATE_KEY = 2;


template<typename Key, typename Tp>
Hashmap<Key,Tp>::Hashmap(int logsize)
: tab_(Hashlib_privatescope_hcreate(logsize)), mem_()
{
}

template<typename Key, typename Tp>
Hashmap<Key,Tp>::~Hashmap()
{
        Hashlib_privatescope_hdestroy(tab_);
}

template<typename Key, typename Tp>
void Hashmap<Key,Tp>::printStatistics()
{
        Hashlib_privatescope_hstat(tab_);
}

template<typename Key, typename Tp>
void Hashmap<Key,Tp>::add(const Key& key, Tp* x)
{
        if(0 == Hashlib_privatescope_hadd(tab_, 
				copy(&key,sizeof(Key)), sizeof(Key), x)) {
                throw DUPLICATE_KEY;
        }
}

template<typename Key, typename Tp>
void Hashmap<Key,Tp>::add(const Key& key, const Tp& x)
{
        if(0 == Hashlib_privatescope_hadd(tab_, copy(&key,sizeof(Key)), 
				sizeof(Key), copy(&x,sizeof(Tp)))) {
                throw DUPLICATE_KEY;
        }
}

template<typename Key, typename Tp>
Tp* Hashmap<Key,Tp>::operator()(const Key& key) const
{
        if(1 == Hashlib_privatescope_hfind(tab_, (ub1* )&key, sizeof(Key))) {
                return (Tp* )Hashlib_privatescope_hstuff(tab_);
        } else {
                throw INVALID_KEY;
        }
}

template<typename Key, typename Tp>
ub1* Hashmap<Key,Tp>::copy(const void* x, size_t nmemb)
{
        ub1* y = (ub1* )mem_.get(nmemb);
        memcpy(y, x, nmemb);

        return y;
}

}

#endif


