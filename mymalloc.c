#include <stdio.h>
#include <stdlib.h>
// #include "mymalloc.h"

#define MEMSIZE 4096
// Including boolean in_use, int chunk_size
// size should be 4*n or will get run-time warning from BehaviorSanitizer
#define HEADERSIZE 8

static char memory[MEMSIZE];

static void memoryInit(){
	if ( *(int*)(memory+1) == 0 )   // chunk size = 0 -> memory is unitialized
	{
		*(int*)(memory+1) = MEMSIZE - HEADERSIZE;   //init chunk size to entire memory
		printf("error here\n");
	}
}

static void createHeader(char* p, int chunk_size){
	*p = 0;   // chunk is NOT in_use
	*(int*)(p+1) = chunk_size;   // update chunk size
}

static void *allocateChunk(char* p, int size, int curr_chunk_size){
	*p = 1;   // Mark chunk in use
	if ( curr_chunk_size - size > HEADERSIZE )   // if enough space to split chunk
	{
		createHeader(p + HEADERSIZE + size, curr_chunk_size - size - HEADERSIZE);
		*(int*)(p+1) = size;   // Change size of chunk
	}
	// not enough space to split chunk, then leave the size as is (wasteful)
	return (void*)(p + HEADERSIZE);
}

void *mymalloc(int size){
	memoryInit();   // check if memory has been initialized, if not, initialize it

	int countCapacity = MEMSIZE;
	char* p = memory;

	while (countCapacity > 0)
	{
		int curr_chunk_size = *(int*)(p+1);

		if ( !(*p) && (curr_chunk_size >= size) )
			// chunk NOT in_use and has sufficient size 
		{
			return allocateChunk(p, size, curr_chunk_size);
		}       
		// chunk is in use or doesn't have sufficient size
		printf("Chunk at %p has payload size of %d bytes\n", p, curr_chunk_size);
		p += HEADERSIZE + curr_chunk_size;   // shift to next chunk
		countCapacity -= HEADERSIZE + curr_chunk_size;
	}
	printf("No space in memory\n");
	return NULL;
}

void myfree(void *ptr){
	printf("Hello World");
}
