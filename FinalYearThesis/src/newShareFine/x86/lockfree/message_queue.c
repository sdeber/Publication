#include "message_queue.h"
#include <stdlib.h>
static struct lockfree_message_queue * TempMessageQueue[PROCNUM];
static cthread * tempThread[PROCNUM];
void * fetch_message(struct lockfree_message_queue * queue)
{
  void * result=NULL;
  int index;
  while(1) { 
	 index=THREAD_SELF->id;	
    queue->waiting_mark = WAITING;
    result=lockfree_fifo_dequeue(queue->messages);
    if(result == NULL) { 
      //go to wait
      if(!JmpBuf_Set(&sched.current[index].c->jmp)) {
	TempMessageQueue[index]=queue;
	tempThread[index]=sched.current[index].c;
	scheduler_jmp2stack(index);
	index=THREAD_SELF->id;
	TempMessageQueue[index]->waiting_thread=tempThread[index];
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
      queue->waiting_mark=NOWAITING;
      return result; 
    }
  }
}

int send_message(void * message, struct lockfree_message_queue * queue)
{
  cthread * thread = NULL;
  int wait_mark;
  if(lockfree_fifo_enqueue(message, queue->messages) == -1)
    return -1;
  // wait_mark = Swap(&queue->waiting_mark,NOWAITING);
  do { 
    if(queue->waiting_thread != NULL) {
      /* Do NOT interchagne the order of the following three lines! Because that 
	 will lead to a potential race condition. For example if we interchange
	 the order, then it is possible that the woke-up thread accesses the message 
	 queue and set itself to queue->waiting_thread again BEFORE this thread sets 
	 queue->waiting_thread to NULL, hence it is lost. 
	 If we interchange the order, then we have to remove the return line, then it is
	 ok since the thread accessing the message queue will reset queue->waiting_mark
	 to WAITING and we are protected by the while loop. */ 
      thread = queue->waiting_thread;
      queue->waiting_thread = NULL;
      if(thread != NULL) {
	      queue->waiting_mark=NOWAITING;
      	scheduler_in(thread);}
      return 0;
    }
  }while(queue->waiting_mark == WAITING);
}

int message_queue_init(struct lockfree_message_queue * queue) {
	queue->waiting_mark = NOWAITING;
	queue->waiting_thread = NULL;
	queue->messages = (struct lockfree_fifo *)malloc(sizeof(struct lockfree_fifo));
	if(queue->messages == NULL)
		return -1;
	lockfree_fifo_init(queue->messages);
	return 0;
}
