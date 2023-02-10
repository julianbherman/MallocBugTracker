#include<stdio.h>
#define MEMSIZE 4096
// Including boolean in_use, int chunk_size
// size should be 4*n or will get run-time warning from BehaviorSanitizer
#define HEADERSIZE 8

static char memory[MEMSIZE];

static void createNextHeader(char* p, int chunk_size){
    p += 1;
    *p = chunk_size;
}

void *mymalloc(int size){
    int countCapacity = MEMSIZE;
    char* p = memory;

    while (countCapacity > 0)
    {
        if (*p == 0) // chunk not in use
        {
            *p = 1; // Mark chunk in use
            *(p+1) = size; // Change size of chunk
            int size_next_chunk = countCapacity - (HEADERSIZE + size);
            if (HEADERSIZE + size < countCapacity)
                createNextHeader(p + HEADERSIZE + size, size_next_chunk);
            return p + HEADERSIZE;
        }       
        // chunk is in use
        // need to shift to next chunk 
        int get_chunk_size = *(p + 1); // get size of chunk
        printf("Chunk starts at %p has payload size of %d\n", p, get_chunk_size);
        p += HEADERSIZE + (size*sizeof(int)); // shift to next chunk
        countCapacity -= HEADERSIZE + size;
    }
    printf("No space in memory\n");
    return NULL;
}

void myfree(void *ptr){
    printf("Hello World");
}
