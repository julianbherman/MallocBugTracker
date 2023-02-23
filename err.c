#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

int main(int argc, char **argv)
{
    int x, *p, *q;
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
        // buffing a few times of malloc & free
        // EXPECTED SUCCESS
        p = (int*) malloc(sizeof(int) * 10);
        free(p);
        q = (int*) malloc(sizeof(int) * 1020); 
        free(q);

        p = (int*) malloc(sizeof(int) * 509);
        q = (int*) malloc(sizeof(int) * 509); 
        break;
    case 5:
        q = (int*) malloc(sizeof(int) * 1020); 
        free(q + 2);
        free(q);
        break;
    case 6:
        free(NULL);
        break;
    }

    
    
    
    return EXIT_SUCCESS;
}
