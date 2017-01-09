#include "smash.h"
#include <stdio.h>
#include <sys/time.h>

#define THREADNUM 5000 
#define YIELDNUM 20000 
#define GRANULARITY 0 
#define EXECNUM 5

cthread * exec[EXECNUM];

void boq() {
  // do nothing
}

void insertremove(cthread * ct) {
    int i = 0;
    cthread * c[THREADNUM];
    
    fprintf(stderr,"In insertremove %d\n",ct->id);

    // threads to be inserted and removed
    for (i = 0; i < THREADNUM; i++) { 
         c[i] = cthread_init(boq,1024,0);
    }
   
    while(1) { 
    // reset i when i becomes equal to THREADNUM
       for (i = 0; i < THREADNUM; i++) {
          fprintf(stderr,"%d, %d inserting %d\n",THREAD_SELF->id,ct->id,i);
          scheduler_in(c[i]); 
          fprintf(stderr,"%d, %d joining %d\n",THREAD_SELF->id,ct->id,i);
          cthread_join(c[i]);
          // thread re-use
          c[i]->joiner = NULL;
       }
    }
}

void cthread_main() {
    int i;
    
    // threads which will do the actual processing
    for (i = 0; i < EXECNUM; i++) { 
         exec[i] = cthread_init(insertremove,512000,0);
    }

    for (i = 0; i < EXECNUM; i++) 
       cthread_run(exec[i]); 

    for (i = 0; i < EXECNUM; i++) 
       cthread_join(exec[i]); 
}



