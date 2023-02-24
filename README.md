# MallocBugTracker
CS 214: Systems Programming  
Professor Menendez  
Long Tran (netID: lht21)  
Julian Herman (netID: jbh113)  



## Makefile Intructions
  ### 'make' (default) will run in non-debug mode
  - To run debug mode, please compile manually using:
      ```
        gcc -c mymalloc.c -DDEBUG=1
        gcc -c err.c
        gcc -o program mymalloc.o err.o
      ```



## Implementation Details

- alignment to 8 bytes is guaranteed
- each chunks' metadata contains information about itself and the previous chunk (essentially a doubly-linked-list)
- structuring of metadata/header:
  - each header size is 8 bytes
  - interpret the header as two contiguous ints (4 bytes each):
    - the first int contains metadata for the current chunk
    - the second int contains metadata for the previous chunk
    - the absolute value of said int represents the corresponding chunk size
      - an absolute value of 0 represents the start or end of memory
    - the sign of said int represents whether the corresponding chunk is in use
      - positive meaning it is in use
      - negative meaning it is NOT in use
- the last 8 bytes of the global memory array remain zero-filled (indicating end of memory)
- initial maximum capacity of payload: 4080 bytes ~ 4080 chars ~ 1020 integers (assuming the macro "memsize" is set to "512" quadwords ~ 4096 bytes)
    - this is because this implementation reserves 8 bytes for the initial header (when there is only one chunk) and 8 bytes at the end of the array to mark memory end -> 4096 - (8 + 8) = 4080 bytes



## Correctness Testing
  ### Use Makefile to run tests:
  ```
    make test
    ./test
  ```
  - NOTE: *tests do not have DEBUG mode*

  ### Library Properties and Corresponding Testing Methods
  1. malloc() ONLY reserves unallocated memory: it does NOT interfere with memory that has already been allocated.  

	#### TESTING METHOD  

        - Malloc() serveral objects
        - Write different values for each object
        - Check again if any object has values changed unexpectedly
        - **EXPECT** NO object has changed values.

  2. if requested amount of space is available, malloc() returns a pointer to the payload of said space. otherwise, malloc() notifies user it does not possess enough space  

	#### TESTING METHOD  

        - malloc() until memory runs out of space
        - **EXPECT** error message warning there is no more space.

  3. free() successfully deallocates memory  

	#### TESTING METHOD  

        - Assign an object of 4 integers
        - Free that object
        - **EXPECT** memory should be marked as NOT_IN_USE

  4. free() coalescing accounts for minor memory fragmentation  

  	#### TESTING METHOD  

        - Assign 2 objects of payload size 48, called A and B (including HEADERSIZE) 
        - Free the object A -> 48 bytes is freed (including HEADERSIZE)
        - Assigning object C of 32 bytes (HEADERSIZE included) -> 16 free bytes in sandwiched by 2 in_use neighbors
        - Assign object D of 48 bytes -> It will be assigned to right of B
        - Free object B 
        - **EXPECT:** Free Coalesce the small memory fragmentation and create a free chunk of 64 bytes (48 + 16).
        - Assign object E of 64 bytes (including HEADERSIZE) (14 integers object)
        - **EXPECT:** address of header E is next to object C.

  5. if free() is called on a pointer to a chunk that has already been freed, there is no change to the metadata and an appropriate error is printed  

  	- *confirmed in err.c*

  6. if free() is called on a pointer that was not provided by malloc(), there is no change to the metadata and an appropriate error is printed. NOTE: this also includes pointers that do not point to the beginning of a chunk.  

	- *confirmed in err.c*

  7. ensures 8 byte alignment such that all pointers returned by malloc() are divisible by 8  

	#### TESTING METHOD  

	- malloc() a bunch of different sized chunks
	- **EXPECT** the pointer address returned is divisible by 8
  	
  8. padding for alignment correctly rounds-up requested size to a multiple of 8 bytes  

        #### TESTING METHOD  

        - Malloc a char object of 6 bytes and a char object of 4 bytes and a char object of 8 bytes
        - Expected Malloc() to gives 3 pointers of 8 bytes
        - Write string of 8 words in objects
        - **EXPECT** the strings still hold

  9. malloc() correctly assigns chunk size to metadata  

	#### TESTING METHOD  

	- Assign an object of 4 integers (16 bytes == 2 quadwords)  
	- **EXPECT** 
  	- First 4 bytes of memory is IN_USE
  	- Size of first 4 bytes of memory == 2 (quadwords)

  10. each call to free() will check both adjacent chunks and coalesce if possible  

        #### TESTING METHOD  

        - Assigning 4 consecutive objects of 8 bytes
        - Check coalesce on right:
          - free() 2nd object and then free 1st object
          - Assign 24 bytes object - Called RightCoalesce
          - **EXPECT** RightCoalesce's location is at the beginning of memory
        - Check coalesce on left:
          - free() 1st object and then free 2nd object
          - Assign 24 bytes object - Called LeftCoalesce
          - **EXPECT** LeftCoalesce's location is at the beginning of memory 
        - Check coalesce on both sides:
          - free() 1st object and 3rd object
          - Assign 40 bytes object - Called BothSideCoalesce
          - **EXPECT** BothSideCoalesce's location is at the beginning of memory  



## Performance Testing
      1. malloc() and immediately free() a 1-byte chunk. repeat 120 times.
      2. use malloc() to allocate 120 1-byte chunks (storing the pointers in an array). then use free() to deallocate the chunks. 
      3. randomly choose between
         - allocating a 1-byte chunk (and storing the pointer in an array)
         - deallocating one of the chunks (if there are any in the array) 
         repeat until you have called malloc() 120 times. then free all remaining allocated chunks.
      4. fill-up 4064 bytes of the array with 254 chunks. then malloc() and immediately free a single chunk 120 times (at the end of memory).
         - this demonstrates a weak-point in the design: malloc() and free() (to verify the pointer) must iterate through each chunk in the array, which is O(n) in worst-case.
      5. fill-up 4064 bytes of the array with 254 chunks. then free() from middle of memory outwards.
         - this tests the robustness and speed of the coalescing feature.



## Design Notes
    - ALL library properties were successfully proved
    - Space waste as a tradeoff for 8-byte alignment:
      - in order to ensure 8 byte alignment, this implementation immediately pads-up / rounds-up the requested space to the nearest multiple of 8
        - this wastes a maximum of 7 bytes in the worst case scenario (where size%8=1)
      - additionally (in the worst case scenario), if the first free chunk in memory of size >= the requested size is EXACTLY 8 bytes larger (than the requested size), then malloc() returns the entire chunk and does NOT split it (8 bytes is only enough space for a header in this implementatio and if it was splitted, would result in a chunk size of 0 which is reserved for memory start and end)  
        - this effectively wastes 8 bytes
      - therefore, maximum space-waste per chunk is 7 + 8 = 15 bytes
    - Iterating through each block in the memory array as a method to validate pointers and find free chunks is costly
      - this is demonstrated in memgrind performance test 4 
