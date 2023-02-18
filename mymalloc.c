#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include "mymalloc.h"

#define MEMSIZE 512	// capacity is 512 QUADWORDS = 4096 BYTES
#define HEADERSIZE 1   // header is 1 QUADWORD = 8 BYTES
#define RED     "\033[31m"      /* Red - Text Color */
#define RESET   "\033[0m"
#ifndef DEBUG
#define DEBUG 0
#endif


/* !!! NEW IMPLEMENTATION !!!
 * -last 8 bytes of memory are all 0 indicating end of memory
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
		*ptr = -(MEMSIZE - 2*HEADERSIZE); // negative means in_use=FALSE
		// NOTE: prev_bloc_size = 0 (memory starts)
		// and the last 8 bytes are 0 (memory ends)
	}
}

/* create a header for a free (NOT in_use) chunk of specified size */
static void createHeader(double* p, int chunk_size, int prev_size){
	int* ptr = (int*)p;
	*ptr = -chunk_size;		// chunk is NOT in_use
	*(ptr+1) = prev_size;		// store size of prev_chunk
					// NOTE: prev_chunk is in_use
	p += HEADERSIZE + chunk_size;	     	
	if ( *(int*)p )			// if next chunk isn't the end of memory
	{
		ptr = (int*)p;		// store size of prev_chunk       	
		*(ptr+1) = -chunk_size; // NOTE: prev_chunk NOT in_use    	
	}
}

/* determine whether to split into two chunks: one in_use and one NOT in_use
 * OR to leave the size as is (already determined to be sufficient)
 * AND in either case, update the header approriately */
static void *allocateChunk(double* p, int size, int curr_chunk_size){
	*(int*)p = curr_chunk_size;   // mark chunk as in_use: set positive size
	int extra_space = curr_chunk_size - size;   // calculate leftover space 
	if ( extra_space >= 1 + HEADERSIZE )   // enough leftover to split?
	{	// then split the chunk
		createHeader(p+HEADERSIZE+size, extra_space-HEADERSIZE, size);
		*(int*)p = size;   // Change size of current chunk and mark as
				   // in_use (size is positive)
	} else {	// if not splitting, update next chunk header if safe
		int* next_hdr_ptr = (int*)(p + HEADERSIZE + curr_chunk_size);
		if ( *next_hdr_ptr ) { *(next_hdr_ptr+1) = curr_chunk_size; }
	}
	// not enough space to split chunk, then leave the size as is (wasteful)
	// wastes a max of ~(15 bytes + initial padding of size) per each chunk
	if (DEBUG > 0) printf("Chunk allocated!\n---Payload at: %p\n", (p+HEADERSIZE));
	return (void*)(p + HEADERSIZE);
}

void *mymalloc(int size, char* file, int line){
	if ( !size ) { return NULL; }	// size is 0

	double* p = memory;
	memoryInit();	// check if memory has been initialized
	int padded_size = padForAlign(size);

	while ( *(int*)p )
	{	// if positive, the chunk is currently in_use
		bool in_use = (*(int*)p) > 0;	

		// size of current chunk = absolute value
		int curr_chunk_size = abs(*(int*)p);	

		if ( !in_use && (curr_chunk_size >= padded_size) )
			// chunk NOT in_use and has sufficient size 
		{
			return allocateChunk(p, padded_size, curr_chunk_size);
		}       
		// chunk is in use or doesn't have sufficient size
		//printf("Chunk in use: %d\nPayload size of: %d quadwords\n",
		//		in_use, curr_chunk_size);
		//printf("Metadata at: %p\nPayload at: %p\n\n", p, (p+1));
		p += HEADERSIZE + curr_chunk_size;   // shift to next chunk
	}

	printf(RED "ERROR IN FILE [%s]\nLINE %d\nNo space in memory\n" RESET, file, line);
	return NULL;
}

/* returns true if ptr was generated by mymalloc(), false otherwise */
static bool ptrValid(void *ptr){
	double* p = memory;

	while ( *(int*)p )
	{
		if ( (*(int*)p) > 0 && ((void*)(p+HEADERSIZE) == ptr) )
		// curr_chunk is in_use and has the same address of given ptr
		{
			return true;	// ptr is valid!
		}
		p += HEADERSIZE + abs(*(int*)p);	// increment p
	}
	return false;	// ptr is NOT valid!
}

void myfree(void *ptr, char* file, int line){

	if ( !ptrValid(ptr) )	// if the ptr was not created by mymalloc()
	{
		printf(RED "ERROR IN FILE [%s]\nLINE %d\nPointer not valid to be freed!\n" RESET, file, line);
		return;
	}
	// ptr WAS created by mymalloc(), proceed to free it
	double* curr_ptr = (double*)ptr;
	int* curr_hdr_ptr = (int*)(curr_ptr-HEADERSIZE);

	int* next_hdr_ptr = (int*)(curr_ptr + *curr_hdr_ptr);
	//printf("next_hdr_ptr: %p\n", next_hdr_ptr);

	if ( (*(curr_hdr_ptr+1) < 0) && (*next_hdr_ptr < 0) )
		// prev_chunk and next_chunk are both free and IN BOUNDS OF MEM
	{
		// update prev_chunk
		int jump_amount = HEADERSIZE + abs(*(curr_hdr_ptr+1));
		int* prev_hdr_ptr = (int*)(curr_ptr - jump_amount - HEADERSIZE);
		*prev_hdr_ptr = -jump_amount - *curr_hdr_ptr + 
				*next_hdr_ptr - HEADERSIZE;

		// update next_next_chunk
		jump_amount = *curr_hdr_ptr + HEADERSIZE + abs(*next_hdr_ptr);
		int* next_next_hdr_ptr = (int*)(curr_ptr + jump_amount);
		if ( *next_next_hdr_ptr )// next_next_hdr_ptr is IN BOUNDS
		{
			*(next_next_hdr_ptr+1) = *prev_hdr_ptr;
		}

		if (DEBUG > 0){
			printf("Free (with coalesce) success: ");
			printf("Both prev_chunk and next_chunk were free!\n");
			printf("---Changed prev_chunk header to have size: %d\n",\
				*prev_hdr_ptr);
		}

	} else if (*(curr_hdr_ptr+1) < 0) {// prev_chunk is free and NOT START 
		// update prev_chunk
		int jump_amount = HEADERSIZE + abs(*(curr_hdr_ptr+1));
		int* prev_hdr_ptr = (int*)(curr_ptr - jump_amount - HEADERSIZE);
		*prev_hdr_ptr = -jump_amount - *curr_hdr_ptr;

		// update next_chunk
		if ( *next_hdr_ptr )	// next_hdr_ptr is IN BOUNDS OF MEM
		{
			*(next_hdr_ptr+1) = *prev_hdr_ptr;
		}
		
		if (DEBUG > 0){
			printf("Free (with coalesce) success: ");
			printf("Only the prev_chunk was free!\n");
			printf("---Changed prev_chunk header to have size: %d\n",\
				*prev_hdr_ptr);
		}

	} else if (*next_hdr_ptr < 0) {// next_chunk is free and NOT END 
		// update curr_chunk
		*curr_hdr_ptr = -*curr_hdr_ptr - HEADERSIZE + *next_hdr_ptr;
		
		// update next_next_chunk
		int* next_next_hdr_ptr = (int*)(curr_ptr + abs(*curr_hdr_ptr));
		if ( *next_next_hdr_ptr )// next_next_hdr_ptr is IN BOUNDS
		{
			*(next_next_hdr_ptr+1) = *curr_hdr_ptr;
		}

		if (DEBUG > 0){
			printf("Free (with coalesce) success: ");
			printf("Only the next_chunk was free!\n");
			printf("---Changed curr_chunk header to have size: %d\n",\
				*curr_hdr_ptr);
		}

	} else {   // neither prev_chunk or next_chunk are free
		*curr_hdr_ptr = -(*curr_hdr_ptr); // mark current chunk as free
 
		// update next_chunk
		if ( *next_hdr_ptr )	// next_hdr_ptr is IN BOUNDS OF MEM
		{	// mark current chunk as free in header of next_chunk
			*(next_hdr_ptr+1) = *curr_hdr_ptr;  
		}

		if (DEBUG > 0){
			printf("Free (without coalesce) success: ");
			printf("Neither adjacent chunk was free!\n");
			printf("---Changed curr_chunk header to have size: %d\n",\
				*curr_hdr_ptr);
		}
	}
}
