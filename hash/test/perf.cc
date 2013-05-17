
#include "hash/hash.hh"

#include <time.h>
// #ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
// #endif /* HAVE_SYS_TIME_H */

static double wtime()
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1e6;
}

static const int N		 = 1000000;
static const int HASHMAP_LOGSIZE = 12;

int main(int argc, char** argv, char** envp)
{
	hash::Hashmap<int,int>	hashmap1(HASHMAP_LOGSIZE), hashmap2(HASHMAP_LOGSIZE);
	int*			arraymap   = new int[RAND_MAX]();
	int*			inversemap = new int[N];
	int*			numbers    = new int[N];
	double			start, end;
	int			n;
	
	for(int i = 0; i < N; ++i) {
		numbers[i] = i;
	}
	
	start = wtime();
	
	srand(0);	
	for(int i = 0; i < N; ++i) {
		inversemap[i] = rand();
		
		hashmap1.add(inversemap[i], &numbers[i]);
		arraymap[inversemap[i]] = i;
	}
	
	end   = wtime();
	printf(" constr1: %16.15f\n", end - start);
	
	start = wtime();
	
	srand(0);	
	for(int i = 0; i < N; ++i) {
		inversemap[i] = rand();
		
		hashmap2.add(inversemap[i], i);
		arraymap[inversemap[i]] = i;
	}
	
	end   = wtime();
	printf(" constr2: %16.15f\n", end - start);
	
	hashmap1.printStatistics();
	
	start = wtime();
	
	n = 0;
	for(int i = 0; i < N; ++i) {
		n += arraymap[inversemap[i]];
	}
	
	end   = wtime();
	printf(" array access: %16.15f\n", end - start);
	
	start = wtime();
	
	n = 0;
	for(int i = 0; i < N; ++i) {
		n += *hashmap1(inversemap[i]);
	}
	
	end   = wtime();
	printf(" hash  access: %16.15f\n", end - start);
	
	return EXIT_SUCCESS;
}

