# MallocBugTracker
A Systems Programming Homework


!!! NEW IMPLEMENTATION !!!

- last 8 bytes of memory are all 0 indicating end of memory
- alignment to 8 bytes is guaranteed
- restructuring of metadata/header:
  - header size is 8 bytes
  - splitting the header into two ints (4 bytes each):
    - FIRST int (data for the current chunk):
      - the absolute value represents current chunk size
      - the sign represents whether it's in_use
        - (negative meaning NOT in_use, positive meaning in_use)
    - SECOND int (data for the previous chunk):
      - the absolute value represents previous chunk size
      - the sign represents whether it's in_use
        - (negative meaning NOT in_use, positive meaning in_use)

- Maximum capacity of payload: 4080 bytes ~ 4080 chars ~ 1020 integers

## Makefile notice:
  ### make (default) will run in non-debug mode
  - To run debug mode, please compile manually using:
      ```
        gcc -c mymalloc.c -DDEBUG=1
        gcc -c err.c
        gcc -o program mymalloc.o err.o
      ```

## Correctness Testing Plan
  ### Run Tests:
  - Use Makefile to run tests:
  - *tests are not implemented DEBUG mode*
  ```
    make test
    ./test
  ```

  - Assert at the start of every first case to check if beginning of memory is not_in_use


  ### What:
    - Singularity check:
      - Check if Malloc() assign first 4 bytes of memory as current chunk size
        #### How:
          - Assign an object of 4 integers
          - **EXPECT** 
            - First 4 bytes of memory is IN_USE
            - Value of first 4 bytes of memory == 2
      - Check if Free() mark first 4 bytes of memory as not in use
        #### How: 
          - Assign an object of 4 integers
          - Free that object
          - **EXPECT**
            - First 4 bytes of memory in NOT_IN_USE
    - Free() mark chunk as not_in_use
        #### How:
          - Assign an object
          - Free object
          - **EXPECT** address of that object as beginning of memory
    - Malloc() preserves unallocated memory
        #### How:
        - Malloc() serveral objects
        - Write different values for each object
        - Check again if any object has values changed unexpectedly
        - **EXPECT** NO object has changed values.
    - Padding Alignment gives multiplication of 8 bytes
        #### How:
        - Malloc a char object of 6 bytes and a char object of 4 bytes and a char object of 8 bytes
        - Expected Malloc() to gives 3 pointers of 8 bytes
        - Write string of 8 words in objects
        - **EXPECT** the strings still holds
    - Free() coalesce with neighbors
        #### How:
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
          - **EXPECT:** BothSideCoalesce's location is at the beginning of memory  
    - Coalescing on Memory Fragmentation:
        #### How:
        - Assign 2 objects of payload size 48, called A and B (including HEADERSIZE) 
        - Free the object A -> 48 bytes is freed (including HEADERSIZE)
        - Assigning object C of 32 bytes (HEADERSIZE included) -> 16 free bytes in sandwiched by 2 in_use neighbors
        - Assign object D of 48 bytes -> It will be assigned to right of B
        - Free object B 
        - **EXPECT:** Free Coalesce the small memory fragmentation and create a free chunk of 64 bytes (48 + 16).
        - Assign object E of 64 bytes (including HEADERSIZE) (14 integers object)
        - **EXPECT:** address of header E is next to object C.

  ### Performance Testing: 
    #### What:  
      - Filling up the array and leave 20 bytes at the end
      Consecutively call malloc at that end-block
