#include "channel.h"
#include <stdlib.h>
#include <string.h>
#define MIN(A,B) ( (A) < (B) ? (A) : (B))
/* Since SMESH already has a simple channel implementation, we just copied a lot of code directly */

#define SCHEDULE_NEXT_THREAD(index) 		\
  if(sched.current[index].c = sched_dequeue())	\
    { sched_jmp(index); }			\
  else {					\
    spinlock(&sched.schedlock);			\
    sched.sleepnum++;				\
    spinrelease(&sched.schedlock);		\
    kernelthread_wait(index);			\
  }						\
  
int channel_set_init(channel_set * chan_set, channel * chan_array[], int chan_num) {
  if(chan_set == NULL || chan_array == NULL)
    return -1;
  chan_set->chan_num = chan_num;
  chan_set->c_lock = 0;
  chan_set->chan_array = chan_array;
  return 0;
}

static channel_set * tempChan_set[PROCNUM];

inline void channel_in_alt(channel_set * chan_set, void * message, int length) {
  int index = THREAD_SELF->id;
  sched.current[index].c->pointer = message;
  sched.current[index].c->chan_length = length;
  //printf("In, trying to get the spin lock\n");
  getSpinlock(&chan_set->c_lock);
  if(!JmpBuf_Set(&sched.current[index].c->jmp)) {
    tempChan_set[index] = chan_set;
    scheduler_jmp2stack(index);
    index =  THREAD_SELF->id;
    chan_set =  tempChan_set[index];
    int count;
    for(count = 0; count < chan_set->chan_num; count++) {
      if(chan_set->chan_array[count]->communicator != NULL) {
	 bcopy((chan_set->chan_array)[count]->communicator->pointer, 
	       sched.current[index].c->pointer, 
	       MIN(chan_set->chan_array[count]->communicator->chan_length, 
		   sched.current[index].c->chan_length));
	 scheduler_in(chan_set->chan_array[count]->communicator);
	 chan_set->chan_array[count]->communicator = NULL;
	 // printf("In, trying to release the spin lock\n");
	 releaseSpinlock(&chan_set->c_lock);
	 sched_jmp(index);
	 return;
      }
    }
    //printf("In, trying to alt wait\n");
    for(count = 0; count < chan_set->chan_num; count++) 
      chan_set->chan_array[count]->communicator =  sched.current[index].c;
    //printf("In, releasing the spin lock\n");
    releaseSpinlock(&chan_set->c_lock);
    //printf("In, the spin lock:%d\n", chan_set->c_lock);
    { SCHEDULE_NEXT_THREAD(index) }
  }
}


static int tempChan_index[PROCNUM];
inline void channel_out_alt(channel_set * chan_set, int chan_index, void * message, int length) {
  int index = THREAD_SELF->id;
  sched.current[index].c->pointer = message;
  sched.current[index].c->chan_length = length;
  //printf("Out, trying to get the spin lock\n");
  getSpinlock(&chan_set->c_lock);
  //printf("Out, got the spin lock\n");
  if(!JmpBuf_Set(&sched.current[index].c->jmp)) {
    tempChan_set[index] = chan_set;
    tempChan_index[index] = chan_index;
    scheduler_jmp2stack(index);
    index =  THREAD_SELF->id;
    chan_set =  tempChan_set[index];
    chan_index = tempChan_index[index];
    //printf("Out, I am here\n");
    cthread * tmp = chan_set->chan_array[chan_index]->communicator;
    if( tmp != NULL) {
      //printf("Out, outputting\n");
      bcopy(sched.current[index].c->pointer,tmp->pointer, MIN(tmp->chan_length, sched.current[index].c->chan_length));
      int count;
      for(count =0; count < chan_set->chan_num;count++) {
	if(chan_set->chan_array[count]->communicator == tmp)
	  chan_set->chan_array[count]->communicator = NULL;
      }
      scheduler_in(tmp);
      releaseSpinlock(&chan_set->c_lock);
      //printf("Out, released the spin lock\n");
      sched_jmp(index);
      return;
    }
    else 
      chan_set->chan_array[chan_index]->communicator =  sched.current[index].c;
    releaseSpinlock(&chan_set->c_lock);
    //printf("Out, released the spin lock\n");
    { SCHEDULE_NEXT_THREAD(index) } 
  }
}
