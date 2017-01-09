#include "scheduler.h"
#include "JmpBuf.h"
#include "kernelthread.h"

scheduler sched;

inline void scheduler_init() {
   int i;
   // dummy head pointer
   cthread * dummy = (cthread *) malloc(sizeof(cthread));

   // to cater for sleeping threads
   sched.threadnum = 0;
   sched.sleepnum = PROCNUM;
   for (i = 0; i < PROCNUM; i++) {
      sched.current[i].c = NULL;
      // inform main thread that thread is ready
      sched.ktready[i] = 0;
      // the following inits the default
      // stack per kt - used to swith to b4 putting a thread
      // back on runqueue
      sched.newstack[i] = (char *) malloc(250000); 
      sched.jmp[i].bp = ((long int*)sched.newstack[i])+(250000>>2)-1;
      sched.jmp[i].sp = sched.jmp[i].bp-1-40;
   }

   dummy->nodeptr = node_init(dummy);
   dummy->id = -1;
   sched.head =  sched.tail = dummy->nodeptr;
#ifdef TWOLOCK
   sched.headlock = (int *) malloc(sizeof(int));
   sched.taillock = (int *) malloc(sizeof(int));
   *sched.headlock = *sched.taillock = 0;
#else
   sched.qlock = (int *) malloc(sizeof(int));
   *sched.qlock = 0;
#endif
   sched.schedlock = 0;
}



