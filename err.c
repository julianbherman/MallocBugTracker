#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

int main(int argc, char **argv)
{
    int x, *p, *r, *q;
    char* c;
    int test = argc > 1 ? atoi(argv[1]) : 0;
    
    switch (test) {
    default:
        puts("Missing or invalid test number");
        return EXIT_FAILURE;
    
    case 1:
        free(&x);
        break;

    case 2:
        p = (int *) malloc(sizeof(int) * 10);
        free(p + 1);
        break;
    
    case 3:
        p = (int *) malloc(sizeof(int) * 10);
        free(p);
        free(p);
        break;

    case 4:
        //Normal malloc testing
        p = (int *) malloc(sizeof(int) * 10);
        printf("Malloc succeeded\nAssigning values for array\n");

        int i;
        int* ptr = p;
        // Assigning p[i] = i
        for (i = 0; i < 10; i++)
        { *ptr++ = i; }

        // print out values of p[i]s
        for (i = 0; i < 10; i++)
            printf("Value of index %d is: %d \n", i, *(p+i));

        free(p);

        // Print values after free
        printf("p after free: \n");
        printf("is header freed? %s \n", (*(p - 2) < 0)? "true" : "false");
        break;

    case 5:
        p = (int*) malloc(sizeof(int) * 10);
        free(p);
        q = (int*) malloc(sizeof(int) * 1020); 
	    break;

    case 6:
        p = (int*) malloc(sizeof(int) * 100);
            q = (int*) malloc(sizeof(int) * 100); 
        free(p);
        r = (int*) malloc(sizeof(int) * 100);
        free(q);
        free(r);
        break;
    case 7:
        // buffing a few times of malloc & free
        // EXPECTED SUCCESS
        p = (int*) malloc(sizeof(int) * 10);
        free(p);
        q = (int*) malloc(sizeof(int) * 1020); 
        free(q);

        p = (int*) malloc(sizeof(int) * 509);
        q = (int*) malloc(sizeof(int) * 509); 
        break;
    case 8:
        // Chars testing
        // EXPECTED SUCCESS
        c = (char*) malloc(sizeof(char) *20);
	printf("The call to malloc returns pointer address: %p\n", c);

        c = "Hello World!";
        printf("%s\n", c);

	printf("The call to free is with pointer address: %p\n", c);
        free(c);
        break;
    }

    
    
    
    return EXIT_SUCCESS;
}
