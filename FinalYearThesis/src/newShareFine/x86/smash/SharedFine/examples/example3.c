#include "cthread.h"
#include "scheduler.h"
#include "channel.h"


#define THREADNUM 10

void boq(cthread * c) {
    fprintf(stderr,"I am thread no %d, %d\n",c->id, THREAD_SELF->id);
}

cthread * c2[THREADNUM];

void other_main() {
    int i;

    fprintf(stderr,"I other main thread %d\n", THREAD_SELF->id);
    for (i = 0; i < THREADNUM;i++) 
           c2[i] = cthread_init(boq,40960,0);

    for (i = 0; i < THREADNUM;i++) 
           cthread_run(c2[i]);
    
    for (i = 0; i < THREADNUM;i++) 
           cthread_join(c2[i]);
    
}

void cthread_main() {
    int i;
    cthread * m;
    cthread * c[THREADNUM];
 
    fprintf(stderr,"I main thread\n");
    m = cthread_init(other_main,40960,0);

    cthread_run(m);

    for (i = 0; i < THREADNUM;i++) 
           c[i] = cthread_init(boq,40960,0);
        
    for (i = 0; i < THREADNUM;i++) 
        cthread_run(c[i]);

    fprintf(stderr,"sleeping on %d\n",THREAD_SELF->id);
    cthread_join(m);
    for (i = 0; i < THREADNUM;i++) 
        cthread_join(c[i]);
 
    
    for (i = 0; i < THREADNUM;i++) {
        free(c[i]->stack);
//        free(c[i]);
    }
   
    for (i = 0; i < THREADNUM;i++) {
        free(c2[i]->stack);
//        free(c2[i]);
    }
}



