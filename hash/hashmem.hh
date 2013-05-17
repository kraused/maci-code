
#ifndef HASH_HASHMEM_HH_INCLUDED
#define HASH_HASHMEM_HH_INCLUDED 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace hash
{

// A memory chunk of size @N
template<size_t N>
class MemChunk
{

public:
	// Default constructor
	MemChunk();
	
	// Returns a new memory chunk
	static MemChunk* New();
	
	// Deletes a memory chunk (provided so that @New and @Delete match)
	static void Delete(MemChunk<N>* p);
	
	// Returns a pointer to the next chunk and @0 if this is the last
	// chunk
	inline       MemChunk* next()
	{
		return succ_;
	}
	inline const MemChunk* next() const
	{
		return succ_;
	}
	
	// Returns a pointer to @mem_
	inline char* mem()
	{
		return mem_;
	}
	
	// Set the @succ_ pointer
	inline void setnext(MemChunk<N>* succ)
	{
		succ_ = succ;
	}
	
private:
	// Successor in the linked list
	MemChunk*		succ_;
	// The memory
	char			mem_[N];
	// Size of the memory in bytes
	static const size_t	SIZE;
};


// A special dynamical memory manager which doesn't need to realloc memory
// It manages a linked list of memory chunks
class Memory
{
	// The chunk size
	static const size_t		MEMCHUNK_SIZE = 1024;
	// Memory chunks
	typedef MemChunk<MEMCHUNK_SIZE>	Chunk;

public:
	// Construct a new @Memory instance
	Memory();
	
	// Clean up
	~Memory();
	
	// Get a contiguous block of size @nmemb. Obviously we must have
	// @nmemb < @MEMCHUNK_SIZE
	void* get(size_t nmemb);

private:
	// The linked list
	Chunk*			list_;
	// The active chunk
	Chunk*			active_;
	// Pointer to the next free byte
	char*			next_;
	// Bytes left in the current chunk
	size_t			left_;
	
public:
	// Exception: No free memory
	static const int	OUT_OF_MEM;
};

template<size_t N>
const size_t MemChunk<N>::SIZE = N;

template<size_t N>
MemChunk<N>::MemChunk()
: succ_(0)
{
}

template<size_t N>
MemChunk<N>* MemChunk<N>::New()
{
        return new MemChunk;
}

template<size_t N>
void MemChunk<N>::Delete(MemChunk<N>* p)
{
        delete p;
}

}

#endif

