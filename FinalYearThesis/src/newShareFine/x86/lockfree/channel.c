#include <string.h>
#include "channel.h"
#include <stdio.h>
#define ENABLING -1
#define READY    -2
#define WAITING  -3

#define SCHEDULE_NEXT_THREAD(index) 		\
  if(sched.current[index].c = sched_dequeue())	\
    { sched_jmp(index); }			\
  else {					\
    spinlock(&sched.schedlock);			\
    sched.sleepnum++;				\
    spinrelease(&sched.schedlock);		\
    kernelthread_wait(index);			\
  }						\
  

#define MIN(A,B) A < B ? A : B
static channel * tempChan[PROCNUM];

void channel_out(channel * chan, void * message, int length) 
{
  int index = THREAD_SELF->id;
  sched.current[index].c->pointer = message;
  sched.current[index].c->chan_length = length;
  if(!JmpBuf_Set(&sched.current[index].c->jmp)) {
    tempChan[index] = chan;
    scheduler_jmp2stack(index);
    // Now we are running on the kernel stack.
    index = THREAD_SELF->id;
    chan = tempChan[index];
    cthread * ctemp = Swap((int32 *)&chan->communicator, (int32)sched.current[index].c);
    if(ctemp == NULL) {
      // The inputting thread has not reached the channel yet, we should wait.
      SCHEDULE_NEXT_THREAD(index)
    }
    else {
      void * pointer = NULL;
      pointer = Swap((int32 *)&ctemp->pointer, READY);
      switch((int32)pointer) {
      case ENABLING:
      case READY: 
	SCHEDULE_NEXT_THREAD(index) 
	break; //Not necessary.
      case WAITING:
	scheduler_in(ctemp);
	SCHEDULE_NEXT_THREAD(index)
      default:
	bcopy(sched.current[index].c->pointer, ctemp->pointer, MIN(ctemp->chan_length,length));
	//jump back to the stack of cthread.
	sched_jmp(index);
      }
    }
  }
}

static channel ** tempChan_Array[PROCNUM];	
static void * tempMessage[PROCNUM];
static int tempChan_Num[PROCNUM];
      
void channel_in_alt(channel *chan_array[], int chan_num, void * message, int length)
{
  int index = THREAD_SELF->id;
  int count;
  cthread * out = NULL;
  //Enable ALT.
  sched.current[index].c->pointer = ENABLING;
  sched.current[index].c->chan_length = length;
  //Enable all participating channels.
  //  printf("Enabling channels\n");
  //printf("Chan_num %d\n",chan_num);
  for(count = 0; count < chan_num; count++) {
    out = Swap((int32*)&chan_array[count]->communicator, (int32)sched.current[index].c);
    if(out != NULL) {
      sched.current[index].c->pointer = READY;
      chan_array[count]->communicator = out;
    }
  }
  //printf("Starting ALT waiting\n");
  //ALT wait.
  sched.current[index].c->temp = NULL;
  //First we save the current context, just in case we have to sleep.
  if(!JmpBuf_Set(&sched.current[index].c->jmp)) {
    tempChan_Array[index] = chan_array;
    tempMessage[index] = message;
    tempChan_Num[index] = chan_num;
    //Jump to the kernel stack.
    scheduler_jmp2stack(index);
    index = THREAD_SELF->id;
    void * temp = Swap((int32*)&sched.current[index].c->pointer, WAITING);
    if(temp == READY) {
      temp = Swap((int32*)&sched.current[index].c->pointer, READY);
      if(temp == READY)
	if(sched.current[index].c = sched_dequeue()) 
	  { sched_jmp(index); }
	else {
	  spinlock(&sched.schedlock);
	  sched.sleepnum++;
	  spinrelease(&sched.schedlock);
	  kernelthread_wait(index);
	}
    }
    else if(temp == ENABLING)
      if(sched.current[index].c = sched_dequeue()) 
	{ sched_jmp(index); }
      else {
	spinlock(&sched.schedlock);
	sched.sleepnum++;
	spinrelease(&sched.schedlock);
	kernelthread_wait(index);
      }
    sched_jmp(THREAD_SELF->id);
  }
  //printf("Disabling channels\n");
  //printf("Chan_num %d\n",chan_num);
  //Disable participating channels and copy message from the chosen channel.
  for(count = 0 ; count < chan_num; count++) {
    out = Swap((int32*)&chan_array[count]->communicator, (int32)NULL);
    if(out == sched.current[index].c) 
      continue;
    else {
      if(sched.current[index].c->temp == NULL) {
	//printf("Copying message\n");
	//This the first channel that is ready for output.
	sched.current[index].c->temp = out;
	//Copy the message.
	bcopy(out->pointer, message, MIN(out->chan_length, length));
	//Communication completed, wake up the outputting thread.
	scheduler_in(out);
      }
      else chan_array[count]->communicator = out;
    }
  }
}
      
