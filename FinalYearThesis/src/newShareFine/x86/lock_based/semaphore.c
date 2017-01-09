#include "semaphore.h"
#include <stdlib.h>
static lockbased_csem * tempCsem[PROCNUM];
static cthread * tempThreads[PROCNUM];

int lockbased_csem_init(lockbased_csem * s,int init_counter) {
  s->lock=FREE;
  s->counter=init_counter;
  s->waiting_queue = (struct simple_fifo*)malloc(sizeof(struct simple_fifo));
  simple_fifo_init(s->waiting_queue);
  return 0;
}

inline int lockbased_csem_wait(lockbased_csem * s) {
  int counter,index=THREAD_SELF->id;
  getSpinlock(&s->lock);
  counter=s->counter;
  if(counter <= 0) {
    //resource is not available, wait.
    if(!JmpBuf_Set(&sched.current[index].c->jmp)) {
      tempThreads[index] = sched.current[index].c;
      tempCsem[index]=s;
      scheduler_jmp2stack(index);
      index=THREAD_SELF->id;
      simple_fifo_enqueue(tempThreads[index],tempCsem[index]->waiting_queue);
      releaseSpinlock(&tempCsem[index]->lock);
      if(sched.current[index].c = sched_dequeue()) 
	{ sched_jmp(index); }
      else {
      //The ready queue is empty, the kernel thread goes to sleep.
	spinlock(&sched.schedlock);
	sched.sleepnum++;
	spinrelease(&sched.schedlock);
	kernelthread_wait(index);
      }
    }
  }
  else {
    s->counter--;
    releaseSpinlock(&s->lock);
  }
  return counter;
}

inline int lockbased_csem_signal(lockbased_csem * s)
{
  int counter;
  int index = THREAD_SELF->id;
  cthread * thread = NULL;
  getSpinlock(&s->lock);
  counter=s->counter;
  if(counter <= 0) {
    //Someone may be waiting on the waiting queue, wake them up.
    thread = (cthread*)simple_fifo_dequeue(s->waiting_queue);
    if(thread == NULL) 
      //No one is waiting
      s->counter++;
    else 
      scheduler_in(thread);
  }
  else
    s->counter++;
  releaseSpinlock(&s->lock);
  return counter;
}
