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


## Correctness Testing Plan
  - ### What:
    - Malloc() preserves unallocated memory
      - #### How:
        - Malloc() serveral objects
        - Write different values for each object
        - Check again if any object has values changed unexpectedly
    - Padding Alignment gives multiplication of 8 bytes
      - #### How:
        - Malloc a char object of 6 bytes and a char object of 4 bytes and a char object of 8 bytes
        - Expected Malloc() to gives 3 pointers of 8 bytes
        - Write string of 8 words in objects
        - Check if the strings still holds
    - Free() mark chunk as not_in_use
      - #### How:
        - Double free on a same object
        - Expected 2nd free to gives error
        - Assign an object, expect address of that object as beginning of memory
    - Free() coalesce with neighbors
      - #### How:
        - Assigning 4 consecutive objects of 8 bytes
        - Check coalesce on right:
          - free() 2nd object and then free 1st object
          - Assign 24 bytes object - Called RightCoalesce
          - Check if RightCoalesce's location is at the beginning of memory
        - Check coalesce on left:
          - free() 1st object and then free 2nd object
          - Assign 24 bytes object - Called LeftCoalesce
          - Check if LeftCoalesce's location is at the beginning of memory 
        - Check coalesce on both sides:
          - free() 1st object and 3rd object
          - Assign 40 bytes object - Called BothSideCoalesce
          - Check if BothSideCoalesce's location is at the beginning of memory  
