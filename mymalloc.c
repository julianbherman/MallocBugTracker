#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include "mymalloc.h"

#define MEMSIZE 512	// capacity is 512 QUADWORDS = 4096 BYTES
#define HEADERSIZE 1   // header is 1 QUADWORD = 8 BYTES

/* !!! NEW IMPLEMENTATION !!!
 * -alignment to 8 bytes is guaranteed
 * -restructuring of metadata/header:
 *  --header size is 8 bytes
 *  --splitting the header into two ints (4 bytes each):
 *    --FIRST int (data for the current chunk):
 *      --the absolute value represents current chunk size
 *      --the sign represents whether it's in_use
 *        --(negative meaning NOT in_use, positive meaning in_use)
 *    --SECOND int (data for the previous chunk):
 *	--the absolute value represents previous chunk size
 *      --the sign represents whether it's in_use
 *        --(negative meaning NOT in_use, positive meaning in_use)
 */

static double memory[MEMSIZE];

/* returns the number of multiples of 8 required to achieve a value >= num */
static int padForAlign(int num){
	if ( num < 8 ) { return 1; };
	int remainder = num%8;
	if (!remainder) { return num/8; }	// num is a mutiple of 8
	return num/8 + 1;
}

/* initializes memory (if not already done so) to the whole block */
static void memoryInit(){
	int* ptr = (int*)memory;
	if ( abs(*ptr) == 0 )	// chunk size = 0 -> memory is unitialized
	{	// init chunk size to entire memory
		*ptr = -(MEMSIZE - HEADERSIZE);	// negative means in_use=FALSE
		*(ptr+1) = 0;	// mark prev_bloc_size = 0 (memory starts)
	}
}

/* create a header for a free (NOT in_use) chunk of specified size */
static void createHeader(double* p, int chunk_size, int prev_size){
	int* ptr = (int*)p;
	*ptr = -chunk_size;			// chunk is NOT in_use
	*(ptr+1) = prev_size;			// store size of prev_chunk
						// NOTE: prev_chunk is in_use
	p += HEADERSIZE + chunk_size;
	ptr = (int*)p;				// update header of next chunk
	*(ptr+1) = -chunk_size;			// store size of prev_chunk
						// NOTE: prev_chunk NOT in_use
}

/* determine whether to split into two chunks: one in_use and one NOT in_use
 * OR to leave the size as is (already determined to be sufficient)
 * AND in either case, update the header approriately */
static void *allocateChunk(double* p, int size, int curr_chunk_size){
	*(int*)p = abs(curr_chunk_size);   // mark chunk as in_use
	int extra_space = curr_chunk_size - size;   // calculate leftover space 
	if ( extra_space >= 1 + HEADERSIZE )   // enough leftover to split?
	{	// then split the chunk
		createHeader(p+HEADERSIZE+size, extra_space-HEADERSIZE, size);
		*(int*)p = size;   // Change size of current chunk and mark as
				   // in_use (size is positive)
	}
	// not enough space to split chunk, then leave the size as is (wasteful)
	// wastes a max of ~(15 bytes + initial padding of size) per each chunk
	printf("Chunk allocated!\n\n");
	return (void*)(p + HEADERSIZE);
}

void *mymalloc(int size){
	if ( !size ) { return NULL; }	// size is 0

	double* p = memory;
	memoryInit();	// check if memory has been initialized
	int padded_size = padForAlign(size);
	int countCapacity = MEMSIZE;

	while (countCapacity > 0)
	{
		bool in_use = (*(int*)p) > 0;	// if positive, the chunk is
						// currently in_use
		int curr_chunk_size = abs(*(int*)p);	// size of current
							// chunk = absolute value

		if ( !in_use && (curr_chunk_size >= padded_size) )
			// chunk NOT in_use and has sufficient size 
		{
			return allocateChunk(p, padded_size, curr_chunk_size);
		}       
		// chunk is in use or doesn't have sufficient size
		printf("Chunk in use: %d\nPayload size of: %d quadwords\n",
				in_use, curr_chunk_size);
		printf("Metadata at: %p\nPayload at: %p\n\n", p, (p+1));
		p += HEADERSIZE + curr_chunk_size;   // shift to next chunk
		countCapacity -= HEADERSIZE + curr_chunk_size;
	}
	printf("No space in memory\n");
	return NULL;
}

void myfree(void *ptr){
	
	double* curr_ptr = (double*)ptr;
	int* curr_hdr_ptr = (int*)(curr_ptr-HEADERSIZE);

	// !!! NEED TO CHECK IF next_hdr_ptr IS IN BOUNDS
	int* next_hdr_ptr = (int*)(curr_ptr + *curr_hdr_ptr);

	// !!! NEED TO CONSIDER when *(curr_hdr_ptr+1)==0 -> AT START OF MEMORY
	if ( (*(curr_hdr_ptr+1) < 0) && (*next_hdr_ptr < 0) )
		// prev_chunk and next_chunk are both free
	{
		// update prev_chunk
		int jump_amount = HEADERSIZE + abs(*(curr_hdr_ptr+1));
		int* prev_hdr_ptr = (int*)(curr_ptr - jump_amount - HEADERSIZE);
		*prev_hdr_ptr = -jump_amount - *curr_hdr_ptr;
		
		// !!! NEED TO CHECK IF next_next_hdr_ptr IS IN BOUNDS
		// update next_next_chunk
		jump_amount = *curr_hdr_ptr + HEADERSIZE + abs(*next_hdr_ptr);
		int* next_next_hdr_ptr = (int*)(curr_ptr + jump_amount);
		*(next_next_hdr_ptr+1) = *prev_hdr_ptr;

	} else if (*(curr_hdr_ptr+1) < 0) {	// prev_chunk is free
		// ...
	} else if (*next_hdr_ptr < 0) {		// next_chunk is free 
		// ...
	} else {   // neither prev_chunk or next_chunk are free (both are in_use)
		*curr_hdr_ptr = -(*curr_hdr_ptr);   // mark current chunk as free
		*(next_hdr_ptr+1) = *curr_hdr_ptr;  // mark current chunk as free
						    // in header of next_chunk
		printf("Free success!\n");
	}
}






















