/*
 * thread run benchmark
 * non cache friendly
 */


#include "smash.h"
#include <stdio.h>
#include <sys/time.h>

#define THREADNUM 25000 
cthread * c[THREADNUM];

void boq(cthread * ct) {
  // do nothing
//  int i;
//  i = i * 42;
}


float calctime(struct timeval * t2, struct timeval *t1) {
   int temp = t2->tv_sec - t1->tv_sec;
   int temp2 = t2->tv_usec - t1->tv_usec;
   printf("%d seconds and %d microseconds\n",temp,temp2);
   return ((temp * 1000000) + temp2); 
}

//char stack[THREADNUM][1024];
int * cachejunk;

void cthread_main(cthread * cmain, int argc, char ** argv) {
    int i;
    struct timeval t1,t2;
    
    gettimeofday(&t1,NULL);
    for (i = 0; i < THREADNUM; i++) { 
	 //cthread_init2(&c[i],boq,stack[i],1024,0);
         c[i] = cthread_init(boq,1024,0);
    }
    gettimeofday(&t2,NULL);
    printf("it took %d microseconds\n", calctime(&t2,&t1));


    printf("This is the main user thread\n");
    printf("%d seconds, %d micro seconds\n",t1.tv_sec,t1.tv_usec);
    gettimeofday(&t1,NULL);
    for (i = 0; i < THREADNUM; i++) {
       cthread_run(c[i]); 
    }
    gettimeofday(&t2,NULL);
    printf("%d seconds, %d micro seconds\n",t2.tv_sec,t2.tv_usec);
  
    printf("it took %f microseconds\n", calctime(&t2,&t1));
    printf("%f nanoseconds per operation\n", 
               (calctime(&t2,&t1)/THREADNUM) * 1000);

    printf("This is the main user thread end\n");
}



