/*
 * context switch performance benchmark - using joins to terminate 
 * thread execution
 */

// if we include even channels, context swith slows down to 62ns
// instead of 57ns 

#include "smash.h"
#include <stdio.h>
#include <sys/time.h>

#define THREADNUM 20 
#define YIELD_NUM (10000000 / THREADNUM)

void boq(cthread * ct) {
    int i;
    for (i = 0; i < YIELD_NUM;i++) {
       cthread_yield();
    }
}


float calctime(struct timeval * t2, struct timeval *t1) {
   int temp = t2->tv_sec - t1->tv_sec;
   int temp2 = t2->tv_usec - t1->tv_usec;
   printf("%d seconds and %d microseconds\n",temp,temp2);
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
        //cthread_init2(&c[i],boq,stack[i],4096,0);
    }


    printf("This is the main user thread\n");
    printf("%d seconds, %d micro seconds\n",t1.tv_sec,t1.tv_usec);
    for (i = 0; i < THREADNUM; i++) 
       cthread_run(c[i]); 
    gettimeofday(&t1,NULL);
    for (i = 0; i < THREADNUM; i++) 
       cthread_join(c[i]); 

    gettimeofday(&t2,NULL);
    printf("%d seconds, %d micro seconds\n",t2.tv_sec,t2.tv_usec);
    printf("%f nanoseconds per operation\n",
               (calctime(&t2,&t1)/10000000) * 1000);

    printf("This is the main user thread end\n");
}



