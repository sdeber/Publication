/*
 * speedup performance benchmark - using joins to terminate 
 * thread execution
 */

#include "smash.h"
#include <stdio.h>
#include <sys/time.h>

#define THREADNUM 20 
#define YIELD_NUM (10000000 / THREADNUM)
#define GRANULARITY 5000 


void boq(cthread * ct) {
    int i,k,p;
    for (i = 0; i < YIELD_NUM;i++) {
       for (k = 0; k < GRANULARITY;k++) p = p + 1;
       cthread_yield();
    }
}


float calctime(struct timeval * t2, struct timeval *t1) {
   int temp = t2->tv_sec - t1->tv_sec;
   int temp2 = t2->tv_usec - t1->tv_usec;
   return ((temp * 1000000) + temp2); 
}
    
//cthread c[THREADNUM];
//char stack[THREADNUM][4096];
cthread * c[THREADNUM];

void cthread_main(cthread * cmain, int argc, char ** argv) {
    int i;
    struct timeval t1,t2;

    for (i = 0; i < THREADNUM; i++) { 
         c[i] = cthread_init(boq,4096,0);
    }


    for (i = 0; i < THREADNUM; i++) 
       cthread_run(c[i]); 
    gettimeofday(&t1,NULL);
    for (i = 0; i < THREADNUM; i++) 
       cthread_join(c[i]); 

    gettimeofday(&t2,NULL);
    fprintf(stderr,"%f microseconds\n", calctime(&t2,&t1));
    fprintf(stderr,"%f microseconds per calc\n", calctime(&t2,&t1)/GRANULARITY);

}



