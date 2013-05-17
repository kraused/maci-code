
#include "hashmem.hh"

namespace hash
{

const int Memory::OUT_OF_MEM = 8;

Memory::Memory()
: list_(Chunk::New()), active_(list_), next_(list_->mem()), left_(MEMCHUNK_SIZE)
{
}

Memory::~Memory()
{
	Chunk* p = list_;
	Chunk* q = 0;
	
	for(; q; p = q) {
		q = p->next();
		Chunk::Delete(p);
	}
}

void* Memory::get(size_t nmemb)
{
	char* p;

	if(nmemb > MEMCHUNK_SIZE) {
		throw OUT_OF_MEM;
	}

	// Enough space left in this chunk
	if(left_ >= nmemb) {
		left_ -= nmemb;
		p      = next_;
		next_ += nmemb;
		
		return p;
	} else {
		active_->setnext(Chunk::New());
		active_	= active_->next();
		
		if(0 == active_) {
			throw OUT_OF_MEM;
		}
		
		// Not optimal, we do not care about the space left in the
		// current chunk
		left_	= MEMCHUNK_SIZE;
		next_   = active_->mem();
		
		return get(nmemb);
	}
}

}

