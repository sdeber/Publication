#include "mutex.h"
/*The resource is free if lock=1, it is locked if lock=0.
The counter stores the number of threads TRYING to get the
mutex, if it gets the mutex successfully, it decrements the
counter, otherwise it puts itself in the waiting queue.
When releaseing the mutex, a thread first check the counter,
if counter==0, then no threads is trying to obtain the mutex,
so it releases the lock. All steps are done atomically.*/

static cmutex_t * tempMutex[PROCNUM];
static cthread * tempThreads[PROCNUM];
void GetMutex(cmutex_t * mutex)
{
  int index = THREAD_SELF->id;
  FetchAndAdd(&(mutex->lock_with_counter.counter),1);
  if(Swap(&(mutex->lock_with_counter.lock),0))
    //Get the mutex successfully.
     FetchAndAdd(&(mutex->lock_with_counter.counter),-1);
  else {
    //The mutex has been locked,adding myself to the waiting queue.
    if(!JmpBuf_Set(&sched.current[index].c->jmp)) {
      tempThreads[index] = sched.current[index].c;
      tempMutex[index] = mutex;
      //Jump to the stack of the kernel thread
      scheduler_jmp2stack(index);
      //revarify the variable "index", it is necessary due to stack change.
      index=THREAD_SELF->id; 
      //put the thread onto the waiting queue.
      lockfree_fifo_enqueue(tempThreads[index],tempMutex[index]->waiting_queue);
      /*Check whether the ready queue of the scheduler is empty, if not dequeue a thread
	and run it, otherwise put the kernel thread to sleep.*/
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
}

void ReleaseMutex(cmutex_t * mutex)
{
  long counter = mutex->lock_with_counter.counter;
  cthread * thread=NULL;
  if(counter==0)
    //Atomically check the waiting queue and release the resource.
    if(CAS2(&mutex->lock_with_counter,
	    0,counter,
	    1,0))
      return;
  do {
    thread = lockfree_fifo_dequeue(mutex->waiting_queue);
  }while(thread==NULL);
  FetchAndAdd(&(mutex->lock_with_counter.counter),-1);
  scheduler_in(thread);
  //sched_jmp(index); no need, since I did not jump to other stacks!
}

int cmutex_init(cmutex_t * m)
{
  m->lock_with_counter.lock=1;
  m->lock_with_counter.counter=0;;
  m->waiting_queue=(struct lockfree_fifo *)malloc(sizeof(struct lockfree_fifo));
  if(m->waiting_queue==NULL)
    return -1;
  lockfree_fifo_init(m->waiting_queue);
  return 0;
}
