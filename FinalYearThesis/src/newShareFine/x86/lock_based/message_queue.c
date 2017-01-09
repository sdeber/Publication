#include "message_queue.h"
#include <stdlib.h>

static struct message_queue * tempQueue[PROCNUM];

int message_queue_init(struct message_queue * queue) {
  struct simple_fifo * m = (struct simple_fifo*)malloc(sizeof(struct simple_fifo));
  if(m == NULL)
    return -1;
  simple_fifo_init(m);
  queue->waiting_thread = NULL;
  queue->messages = m;
  queue->lock = FREE;
  return 0;
}

int send_message(void * message, struct message_queue * queue) {
  getSpinlock(&queue->lock);
  simple_fifo_enqueue(message, queue->messages);
  if(queue->waiting_thread != NULL) {
    scheduler_in(queue->waiting_thread);
    queue->waiting_thread = NULL;
  }
  releaseSpinlock(&queue->lock);
  return 0;
}

void * fetch_message(struct message_queue * queue) {
  void * message = NULL;
  int index = THREAD_SELF->id;
  getSpinlock(&queue->lock);
  do {
  message = simple_fifo_dequeue(queue->messages);
  if(message == NULL) { 
    //Message queue is empty, wait
    if(!JmpBuf_Set(&sched.current[index].c->jmp)) {
      tempQueue[index] = queue;
      scheduler_jmp2stack(index);
      index=THREAD_SELF->id;
      tempQueue[index]->waiting_thread = sched.current[index].c;
      releaseSpinlock(&tempQueue[index]->lock);
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
  else 
    {
      releaseSpinlock(&queue->lock);
      return message;
    }
  }while(message == NULL);
}
