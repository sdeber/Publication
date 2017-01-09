#include "semaphore.h"

static struct c_semaphore * tempSem[PROCNUM];
static cthread * tempThreads[PROCNUM];
int csem_init(struct c_semaphore * sem, int init_count)
{
  struct lockfree_fifo * waiting_queue = (struct lockfree_fifo*)malloc(sizeof(struct lockfree_fifo));
  if(waiting_queue == NULL)
    return -1;
  sem->counter=init_count;
  sem->waiting_queue = waiting_queue;
  lockfree_fifo_init(sem->waiting_queue);
  return 0;
}

int csem_wait(struct c_semaphore * sem)
{
  int index = THREAD_SELF->id;
  long counter;
  /* Maybe it is better to use FetchAndAdd here, but using CAS does have advantage.
     In some cases, this can avoid unecessary waiting*/ 
  do {
    counter = sem->counter;
  }while(!CAS(&sem->counter,counter,counter-1));
  if(counter <= 0)
    if(!JmpBuf_Set(&sched.current[index].c->jmp)) {
      tempThreads[index] = sched.current[index].c;
      tempSem[index]=sem;
      scheduler_jmp2stack(index);
      index=THREAD_SELF->id;
      lockfree_fifo_enqueue(tempThreads[index], tempSem[index]->waiting_queue); 
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
  return counter;
}

int csem_signal(struct c_semaphore * sem)
{
  long counter;
  cthread * thread = NULL;
  do {
    while(1) {
      counter=sem->counter;
      if(counter < 0)
	//There are threads waiting for the semaphore.
	{
	  thread = lockfree_fifo_dequeue(sem->waiting_queue);
	  if(thread == NULL)
	    /*Did not get any thread from the waiting queue, but it does not mean
	      that the waiting queue is empty, maybe someone is enqueuing.*/
	    continue;
	  else { //successfully get a waiting thread.
	    scheduler_in(thread);
	    FetchAndAdd(&sem->counter,1);
	    return counter;
	  }
	}
      else //The semaphore is still available, jump out of the inner while loop.
	break;
    }
  }while(!CAS(&sem->counter,counter,counter+1));
  return counter;
}
