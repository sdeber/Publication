#include "smash.h"
#include <stdio.h>
#include <sys/time.h>

#define THREADNUM 5 
#define YIELDNUM 200000 

#include "example.h"

void boq(cthread * ct) {
    int i;
    for (i = 0; i < YIELDNUM;i++) {
       cthread_yield2();
    }
}


float calctime(struct timeval * t2, struct timeval *t1) {
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
    printf("%d seconds, %d micro seconds\n",t2.tv_sec,t2.tv_usec);
    printf("%f nanoseconds per operation\n",
               (calctime(&t2,&t1)/(YIELDNUM * THREADNUM)) * 1000);

/*
    gettimeofday(&t1,NULL);
    for (i = 0; i < 10000000; i++) {
       yielding(); 
    }
    gettimeofday(&t2,NULL);
    printf("%d seconds, %d micro seconds\n",t2.tv_sec,t2.tv_usec);
    printf("%f nanoseconds per operation\n",
               (calctime(&t2,&t1)/10000000) * 1000);
*/

}



