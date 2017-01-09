#include "smash.h"
#include <stdio.h>

#define THREADNUM 4000

void boq(cthread * ct, int i) {
    fprintf(stderr,"I am thread number %d. pid is %d\n",i,THREAD_SELF->id);
}


void cthread_main() {
    int i;
    cthread * c[THREADNUM];
    printf("This is the main user thread\n");
    for (i = 0; i < THREADNUM; i++) { 
         c[i] = cthread_init(boq,40960,1,i);
    }

    for (i = 0; i < THREADNUM; i++) 
       cthread_run(c[i]); 

    for (i = 0; i < THREADNUM; i++) 
       cthread_join(c[i]);

    fprintf(stderr,"This is the main user thread 2\n");
    fprintf(stderr,"This is the main user thread 3\n");
}



