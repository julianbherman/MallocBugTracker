#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

int main(int argc, char **argv)
{
    int x, *p;
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
        break;

    case 5:
        p = (int*) malloc(sizeof(int) * 10);
        int* q;
        q = (int*) malloc(sizeof(int) * 1024); 
	break;

    case 6:
	int* r;
	p = (int*) malloc(sizeof(int) * 100);
        q = (int*) malloc(sizeof(int) * 100); 
	free(p);
	r = (int*) malloc(sizeof(int) * 100);
	free(q);
	free(r);
	break;
    }
    
    return EXIT_SUCCESS;
}
