/* 
 * performance indicator for execution of thread 
 * cache unfriendly version
 */

#include "smash.h"
#include <stdio.h>
#include <sys/time.h>

#define THREADNUM 25000 

void boq(cthread * ct) {
  // do nothing
  int i;
  i *= 42;
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

    gettimeofday(&t1,NULL);
    for (i = 0; i < THREADNUM; i++) 
       cthread_run(c[i]); 
    for (i = 0; i < THREADNUM; i++) 
       cthread_join(c[i]); 
    gettimeofday(&t2,NULL);
  
    fprintf(stderr,"%f nanoseconds per operation\n",
               (calctime(&t2,&t1)/(THREADNUM)) * 1000);
    fprintf(stderr,"%f nanoseconds per operation\n", calctime(&t2,&t1));

}



