#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include "mymalloc.h"

long count_time(struct timeval start, struct timeval end){
    return (end.tv_sec * 1000000 + end.tv_usec) -
        (start.tv_sec * 1000000 + start.tv_usec);
}

void print_time(long time){
    printf("Time taken is : %ld  msecs\n", time);
}
void print_time_double(double time){
    printf("Time taken is : %f  msecs\n", time); 
}

void test1(){
    // malloc and free, 120 times
    struct timeval start, end;
    char* p;
    int repeat = 50;
    long average_time = 0;
    for (int run = 0; run < repeat; run++)
    {
        gettimeofday(&start, NULL);
        for (int i = 0; i < 120; i++)
        {
            p = (char*) malloc(sizeof(char)*1);
            free(p);   
        }  
        gettimeofday(&end, NULL);
        average_time += count_time(start,end);
        //print_time(count_time(start,end));
    }
    printf("The average time is: \n");
    print_time_double(average_time/(double)repeat);   
}
void test2(){
    // malloc 120 times and then free all
    struct timeval start, end;
    char *arr[150], *p;
    int repeat = 50;
    long average_time = 0;
    for (int run = 0; run < repeat; run++)
    {
        gettimeofday(&start, NULL);
        for (int i = 0; i < 120; i++)
        {
            p = (char*) malloc(sizeof(char)*1);
            arr[i] = p;
        }
        for (int i = 0; i < 120; i++)
        {
            free(arr[i]);
        }  
        gettimeofday(&end, NULL);
        average_time += count_time(start,end);
        //print_time(count_time(start,end));
    }
    printf("The average time is: \n");
    print_time_double(average_time/(double)repeat);   
}
void test3(){
    // malloc and free, 120 times
    struct timeval start, end;
    char *arr[5000], *p;
    short repeat = 50, is_malloc;
    long average_time = 0;
    for (int run = 0; run < repeat; run++)
    {
        gettimeofday(&start, NULL);
        short malloc_counter = 0, arr_count = -1;
        while(malloc_counter < 120)
        {
            is_malloc = rand() % 2; // random with proba 1/2
            if (is_malloc) {
                p = (char*) malloc(sizeof(char)*1);
                arr_count++;
                arr[arr_count] = p;
                malloc_counter++;
            }
            else {
                if (arr_count >= 0)
                {
                    free(arr[arr_count]);
                    arr_count--;
                }
            }
        }  
        while (arr_count >= 0)
            free(arr[arr_count--]);
        gettimeofday(&end, NULL);
        average_time += count_time(start,end);
        //print_time(count_time(start,end));
    }
    printf("The average time is: \n");
    print_time_double(average_time/(double)repeat);   
}
void test4(){
    // Fill up 4064 bytes of memory; Malloc and free 120 time
    struct timeval start, end;
    char *arr[260], *p;
    int repeat = 50;
    long average_time = 0;

    for (int run = 0; run < repeat; run++)
    {
        for (int i = 0; i < 254; i++) 
        {
            p = (char*) malloc(sizeof(char)*1);
            arr[i] = p;
        }
        gettimeofday(&start, NULL);
        for (int i = 0; i < 120; i++)
        {
            p = (char*) malloc(sizeof(char)*1);
            free(p);   
        }  
        gettimeofday(&end, NULL);
        for (int i = 0; i < 254; i++) 
            free(arr[i]);
        average_time += count_time(start,end);
        //print_time(count_time(start,end));
    }
    printf("The average time is: \n");
    print_time_double(average_time/(double)repeat);   
}
void test5(){
    // Fill up 4064 bytes of memory; Malloc and free 120 time
    struct timeval start, end;
    char *arr[260], *p;
    int repeat = 50;
    long average_time = 0;

    for (int run = 0; run < repeat; run++)
    {
        for (int i = 0; i < 254; i++) 
        {
            p = (char*) malloc(sizeof(char)*1);
            arr[i] = p;
        }
        gettimeofday(&start, NULL);
        for (int i = 0; i < 120; i++)
        {
            p = (char*) malloc(sizeof(char)*1);
            free(p);   
        }  
        gettimeofday(&end, NULL);
        for (int i = 0; i < 254; i++) 
            free(arr[i]);
        average_time += count_time(start,end);
        //print_time(count_time(start,end));
    }
    printf("The average time is: \n");
    print_time_double(average_time/(double)repeat);   
}
int main(){
    printf("TEST 1: Malloc and immediately free 120 times\n");
    test1();
    printf("TEST 2: Malloc 120 times then free all\n");
    test2();
    printf("TEST 3: Randomly choose between Malloc and Free until Malloc() 120 times, then free all\n");
    test3(); 
    printf("TEST 4: Fill up memory 4064 bytes; malloc and free 120 times at the end of memory\n");
    test4();
    printf("")
    printf("\n\n");
    return 0;
}