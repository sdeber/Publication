#include "smash.h"
#include <stdio.h>
#include <sys/time.h>

#define THREADNUM 50 
#define YIELDNUM 20000 
#define GRANULARITY 0 

void boq(cthread * ct) {
    int i,k;
    for (i = 0; i < YIELDNUM;i++) {
       // do something
       for (k = 0; k < GRANULARITY;k++); 
       cthread_yield();
    }
}


long calctime(struct timeval * t2, struct timeval *t1) {
   int temp = t2->tv_sec - t1->tv_sec;
   int temp2 = t2->tv_usec - t1->tv_usec;
   return ((temp * 1000000) + temp2); 
}

void cthread_main() {
    int i;
    cthread * c[THREADNUM];
    struct timeval t1,t2;

    for (i = 0; i < THREADNUM; i++) { 
         c[i] = cthread_init(boq,250000,0);
    }


    gettimeofday(&t1,NULL);
    for (i = 0; i < THREADNUM; i++) 
       cthread_run(c[i]); 

    for (i = 0; i < THREADNUM; i++) 
       cthread_join(c[i]); 

    //while(1);
    gettimeofday(&t2,NULL);
    fprintf(stderr,"it took %d microseconds\n", calctime(&t2,&t1));
}



