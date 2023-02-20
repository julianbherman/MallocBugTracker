#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mymalloc.c"

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define malloc(s)   mymalloc(s, __FILE__, __LINE__)
#define free(p)     myfree(p, __FILE__, __LINE__)


bool test1() {
    // Malloc preserves unallocated memory
    int* a = (int*) malloc(sizeof(int) *10);
    int* b = (int*) malloc(sizeof(int) *10);
    int* c = (int*) malloc(sizeof(int) *10);
    int* d = (int*) malloc(sizeof(int) *10);
    int* e = (int*) malloc(sizeof(int) *10);

    int i;
    for (i = 0; i < 10; i++)
    {
        a[i] = 1;
        b[i] = 2;
        c[i] = 3;
        d[i] = 4;
        e[i] = 5;
    }

    bool failed = false;
    for (i = 0; i < 10; i++)
        if (a[i] != 1) {
           return failed;
        }
    for (i = 0; i < 10; i++)
        if (b[i] != 2) {
           return failed;
        }
    for (i = 0; i < 10; i++)
        if (c[i] != 3) {
           return failed;
        }
    for (i = 0; i < 10; i++)
        if (d[i] != 4) {
           return failed;
        }
    for (i = 0; i < 10; i++)
        if (e[i] != 5) {
           return failed;
        }

    free(a);
    free(b);
    free(c);
    free(d);
    free(e);

    return !failed;
}

bool test2(){
    char* c1 = (char*) malloc(sizeof(char)*6);
    char* c2 = (char*) malloc(sizeof(char)*4);
    char* c3 = (char*) malloc(sizeof(char)*8);
    for (int i = 0; i < 8; i++)
    {
        c1[i] = 'a' + i;
        c2[i] = 'i' + i;
        c3[i] = 'q' + i;
    }

    bool failed = false;
    for (int i = 0; i < 8; i++){
        if (c1[i] != ('a' + i))
            return failed;
        if (c2[i] != ('i' + i))
            return failed;
    }

    free(c1);
    free(c2);
    free(c3);

    return !failed;
}

bool test3() {
    int* q = (int*) malloc(sizeof(int)*10);
    free(q);
    double* p = (double*) malloc(sizeof(double)*10);
    //printf("chunk address: %p \n memory address: %p\n", (p-HEADERSIZE), memory);
    bool passed = ((p - HEADERSIZE) == memory);
    free(p);
    return (passed);
}

bool test4_1() {
    int* left = (int*) malloc(sizeof(int)*2); // 8bytes
    int* mid = (int*) malloc(sizeof(int)*2); // 8bytes
    int* right = (int*) malloc(sizeof(int)*2);
    free(mid);
    free(left);
    int* new_left = (int*) malloc(sizeof(int)*6);
    bool passed = (new_left - HEADERSIZE*2 == (int*)memory); 
    //printf("new ptr is: %p \n memory is at: %p \n", new_left, memory);
    free(new_left);
    free(right);
    return (passed);
}
bool test4_2() {
    int* left = (int*) malloc(sizeof(int)*2); // 8bytes
    int* mid = (int*) malloc(sizeof(int)*2); // 8bytes
    int* right = (int*) malloc(sizeof(int)*2);
    free(left);
    free(mid);
    int* new_ptr = (int*) malloc(sizeof(int)*6);
    bool passed = (new_ptr - HEADERSIZE*2 == (int*)memory); 
    //printf("new ptr is: %p \n memory is at: %p \n", new_ptr, memory);
    free(new_ptr);
    free(right);
    return (passed);
}
bool test4_3() {
    int* left = (int*) malloc(sizeof(int)*2); // 8bytes
    int* mid = (int*) malloc(sizeof(int)*2); // 8bytes
    int* right = (int*) malloc(sizeof(int)*2); // 8bytes
    int* q = (int*) malloc(sizeof(int)*2); // 8bytes
    free(left);
    free(right);
    free(mid);
    int* new_ptr = (int*) malloc(sizeof(int)*10);
    bool passed = (new_ptr - HEADERSIZE*2 == (int*)memory); 
    //printf("new ptr is: %p \n memory is at: %p \n", new_ptr, memory);
    free(new_ptr);
    free(q);
    return (passed);
}
int main() {
    printf("TEST 1: Malloc preserves unallocated memory\n");
    test1() ? printf(GREEN "PASSED\n" RESET) : printf(RED "FAILED\n" RESET);
    printf("TEST 2: Padding Alignment gives chunks of 8 bytes\n");
    test2() ? printf(GREEN "PASSED\n" RESET) : printf(RED "FAILED\n" RESET); 
    printf( "TEST 3: Free mark chunk not_in_use\n");
    test3() ? printf(GREEN "PASSED\n" RESET) : printf(RED "FAILED\n" RESET); 
    printf("TEST 4: Check Coalesces\n");
    printf("    ---TEST4.1: on right: "); test4_1() ? printf(GREEN "PASSED\n" RESET) : printf(RED "FAILED\n" RESET);   
    printf("    ---TEST4.2: on left: "); test4_2() ? printf(GREEN "PASSED\n" RESET) : printf(RED "FAILED\n" RESET);  
    printf("    ---TEST4.3: on left: "); test4_3() ? printf(GREEN "PASSED\n" RESET) : printf(RED "FAILED\n" RESET);    
    return 0;
}



