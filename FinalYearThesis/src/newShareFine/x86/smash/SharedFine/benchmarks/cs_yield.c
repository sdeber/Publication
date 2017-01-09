/*
 * context switch performance benchmark using yield only
 * useful for calculating non-cached yield
 * this version is not cache friendly
 */

#include "smash.h"
#include <stdio.h>
#include <sys/time.h>

#define THREADNUM 25000 
#define NUM_YIELDS 10000000


long count = 0;

void boq(cthread * ct) {
    while (count < NUM_YIELDS) {
       cthread_yield();
       count++;
    }
}

float calctime(struct timeval * t2, struct timeval *t1) {
   int temp = t2->tv_sec - t1->tv_sec;
   int temp2 = t2->tv_usec - t1->tv_usec;
   printf("%d seconds and %d microseconds\n",temp,temp2);
   return ((temp * 1000000) + temp2); 
}

void cthread_main(cthread * cmain, int argc, char ** argv) {
    int i;
    cthread * c[THREADNUM];
    struct timeval t1,t2;

    for (i = 0; i < THREADNUM; i++) { 
         c[i] = cthread_init(boq,1024,0);
    }


    printf("This is the main user thread\n");
    for (i = 0; i < THREADNUM; i++) 
       cthread_run(c[i]); 
    gettimeofday(&t1,NULL);
    for (count = 0; count < NUM_YIELDS; count++) 
       cthread_yield();
    gettimeofday(&t2,NULL);
  
    printf("it took %d microseconds\n", calctime(&t2,&t1));
    printf("it took %f microseconds\n", calctime(&t2,&t1));
    printf("%f nanoseconds per operation\n",
               (calctime(&t2,&t1)/(NUM_YIELDS)) * 1000);

    printf("This is the main user thread end\n");
}



