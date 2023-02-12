# MallocBugTracker
A Systems Programming Homework


!!! NEW IMPLEMENTATION !!!

-alignment to 8 bytes is guaranteed
-restructuring of metadata/header:
 --header size is 8 bytes
 --splitting the header into two ints (4 bytes each):
   --FIRST int (data for the current chunk):
     --the absolute value represents current chunk size
     --the sign represents whether it's in_use
       --(negative meaning NOT in_use, positive meaning in_use)
   --SECOND int (data for the previous chunk):
     --the absolute value represents previous chunk size
     --the sign represents whether it's in_use
       --(negative meaning NOT in_use, positive meaning in_use)
