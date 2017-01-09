/* 
 * commstime for smash - adapted from MESH commstime
 */

#include <stdio.h>
#include "smash.h"
#include "channel.h"
#include <sys/time.h>

#define THREADNUM 25000 
#define EXTRANUM PROCNUM 

cthread * in[THREADNUM];
cthread * out[THREADNUM];
cthread * extra[EXTRANUM];
channel * chan[THREADNUM];

float calctime(struct timeval * t2, struct timeval *t1) {
   int temp = t2->tv_sec - t1->tv_sec;
   int temp2 = t2->tv_usec - t1->tv_usec;
   return ((temp * 1000000) + temp2);
}

void thread_extra() {
   int i,k = 0;
   while(1) {
//      for (i = 0; i < 10000; i++) 
  //       k = k + 1;
      cthread_yield();
   }
}


void thread_in(cthread *p, int i) {
   int value;
   channel_in(chan[i],(void *) &value, sizeof(int));
}

void thread_out(cthread *p, int i) {
   int value;
   channel_out(chan[i],(void *) &value, sizeof(int));
}


void cthread_main(cthread *c, int argc, char** argv) {
    int i;
    struct timeval t1,t2;

    for (i = 0; i < THREADNUM; i++) {
         in[i] = cthread_init(thread_in,1024,1,i);
         out[i] = cthread_init(thread_out,1024,1,i);
         chan[i] = channel_init();
    }

    for (i = 0; i < EXTRANUM; i++) 
         extra[i] = cthread_init(thread_extra,1024,0);
    
    for (i = 0; i < EXTRANUM; i++) 
         cthread_run(extra[i]);

    gettimeofday(&t1,NULL);
    for (i = 0; i < THREADNUM; i++) {
         cthread_run(in[i]);
         cthread_run(out[i]);
    }

    for (i = 0; i < THREADNUM; i++) {
         cthread_join(in[i]);
         cthread_join(out[i]);
    }
    gettimeofday(&t2,NULL);

    fprintf(stderr,"%f nanoseconds per operation\n",
               (calctime(&t2,&t1)/(THREADNUM)) * 1000);
    fprintf(stderr,"%f microseconds\n", calctime(&t2,&t1));
//    while(1);
}
