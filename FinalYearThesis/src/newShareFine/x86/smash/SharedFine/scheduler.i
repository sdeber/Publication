#include "scheduler.h"
#include "JmpBuf.h"
#include "kernelthread.h"


// insert thread into scheduler
inline void scheduler_in(cthread * c) {
   int i;

   // protect currents and sleepnum variable
   spinlock(&sched.schedlock);
   if (sched.sleepnum == 0) {
      spinrelease(&sched.schedlock);
      sched_enqueue(c);
      return; 
   } else {
      for (i = 0;i < PROCNUM;i++) {
          if (!sched.current[i].c) {
              static struct sembuf semosleep[1]={{0,1,0}};

              sched.sleepnum--;
              spinrelease(&sched.schedlock);
              sched.current[i].c = c;
              semop(sched.sem_id[i], semosleep,1);
              return; // exit loop
          }
      } // end of for loop
   } // end of is sched.sleepnum == 0
   // we get here only if there is no one to wake up
   spinrelease(&sched.schedlock);
}

inline void scheduler_schedule(int index) {
   JmpBuf_Jmp(&sched.current[index].c->jmp);
}


