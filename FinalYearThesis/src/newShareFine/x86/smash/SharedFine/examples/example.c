#include "smash.h"
#include "cthread.h"
#include "scheduler.h"

void boq1(cthread * ct, int i) {
    printf("This is boq %d\n",i);
    printf("This is boq1 part 1\n");
//    cthread_yield();
    printf("This is boq1 part 2\n");
}

void boq2() {
    printf("This is boq2 part 1\n");
//    cthread_yield();
    printf("This is boq2 part 2\n");
}

void boq3() {
    printf("This is boq3 \n");
}

void cthread_main() {
    int i = 10;
    cthread * c1, *c2 ,*c3;
    c1 = cthread_init(boq1,409600,2,10,12);
    c2 = cthread_init(boq2,409600,0);
    c3 = cthread_init(boq3,409600,0);

    cthread_run(c1); 
//    cthread_run(c2); 
//    cthread_run(c3); 
    printf("This is the main user thread\n");
//    cthread_yield();
    printf("This is the main user thread 2\n");
//    cthread_yield();
    // this is here for convenience sake
//    cthread_yield();
//    cthread_join(c2);
    cthread_join(c1);
    printf("This is the main user thread 3 %d\n", kernelthread_num());
}



