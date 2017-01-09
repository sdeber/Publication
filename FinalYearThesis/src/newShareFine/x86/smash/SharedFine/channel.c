/* 
 * The channel implemenations
 */ 

#include "scheduler.h"
#include "channel.h"
#include "JmpBuf.h"
#include <string.h>

#define MIN(A,B) ( (A) < (B) ? (A) : (B))

// faster than strncpy by some 25%
inline void messcpy(char * dest, char * cons, int num) {
   int i;
   for (i = 0; i < num; i++) {
      dest[i] = cons[i];
   }
}

typedef unsigned long int32;

inline int CAS(int32 *loc,int32 old, int32 new) {
   unsigned char success;
   asm volatile("lock ; cmpxchg %4,%3 ; sete %0; xorl %%eax,%%eax"
                        :"=q" (success), "=a" (loc)
                        :"a" (old), "m" (* loc), "r" (new)
                        :"memory");
   return (int) success;
}



// channel init
channel * channel_init() {
   channel * chan = (channel *) malloc(sizeof(channel));
   chan->communicator = NULL;
   chan->clock = 0;
   return chan;
}



#ifdef LOCKFREE

#define comms_yield(NUM)                          \
      if (sched.current[NUM].c = sched_dequeue()) { \
         sched_jmp(NUM);                          \
      } else {                                    \
         spinlock(&sched.schedlock);              \
         sched.sleepnum++;                        \
         spinrelease(&sched.schedlock);           \
         kernelthread_wait(NUM);                  \
      }                                           \

channel * chantemp[PROCNUM];

inline void channel_in(channel * chan, void * message, int length) {
   sched.current[THREAD_SELF->id].c->chan_length = length;
   sched.current[THREAD_SELF->id].c->pointer = message;
   if (!JmpBuf_Set(&sched.current[THREAD_SELF->id].c->jmp)) {   
      chantemp[THREAD_SELF->id] = chan;
      scheduler_jmp2stack(THREAD_SELF->id);  
      chan = chantemp[THREAD_SELF->id];
      if (!CAS((int32 *)&chan->communicator,(int32)NULL
                   ,(int32)sched.current[THREAD_SELF->id].c)) {
         cthread * ctemp = chan->communicator;
         chan->communicator = NULL;
         messcpy((char *) sched.current[THREAD_SELF->id].c->pointer,
                 (char *) ctemp->pointer, 
           MIN(sched.current[THREAD_SELF->id].c->chan_length,ctemp->chan_length));
         // re_insert thread onto run queue
         scheduler_in(ctemp);
         sched_jmp(THREAD_SELF->id); //why
      } else {
         switch(THREAD_SELF->id) {
         case 0: comms_yield(0); break;
         case 1: comms_yield(1); break;
         case 2: comms_yield(2); break;
         case 3: comms_yield(3); break;
         }
      } // endif chan->communicator
   } // endif save context
}

// this out does not support alt
inline void channel_out(channel * chan, void * message, int length) {
   // if channel already exists just pass message
   sched.current[THREAD_SELF->id].c->chan_length = length;
   sched.current[THREAD_SELF->id].c->pointer = message;
   if (!JmpBuf_Set(&sched.current[THREAD_SELF->id].c->jmp)) {   
      chantemp[THREAD_SELF->id] = chan;
      scheduler_jmp2stack(THREAD_SELF->id);       
      chan = chantemp[THREAD_SELF->id];
      if (!CAS((int32 *)&chan->communicator,(int32) NULL,
                  (int32)sched.current[THREAD_SELF->id].c)) {
         cthread * ctemp = chan->communicator;
         chan->communicator = NULL;
         messcpy((char *) ctemp->pointer, 
                 (char *) sched.current[THREAD_SELF->id].c->pointer,
           MIN(sched.current[THREAD_SELF->id].c->chan_length,ctemp->chan_length));
         scheduler_in(ctemp);
         sched_jmp(THREAD_SELF->id);
      } else {
         switch(THREAD_SELF->id) {
         case 0: comms_yield(0); break;
         case 1: comms_yield(1); break;
         case 2: comms_yield(2); break;
         case 3: comms_yield(3); break;
         }
      } // endif chan->communicator
   } // endif save context
}


#else // LOCKFREE

channel * chantemp[PROCNUM];

// This is the same as the normal yield - again slightly faster
#define comms_yield(NUM)                          \
   if (!JmpBuf_Set(&sched.current[NUM].c->jmp)) {   \
      chantemp[NUM] = chan;                       \
      scheduler_jmp2stack(NUM);                   \
      chan = chantemp[NUM];                       \
      chan->communicator = sched.current[NUM].c;    \
      spinrelease(&chan->clock);                  \
      if (sched.current[NUM].c = sched_dequeue()) { \
         sched_jmp(NUM);                          \
      } else {                                    \
         spinlock(&sched.schedlock);              \
         sched.sleepnum++;                        \
         spinrelease(&sched.schedlock);           \
         kernelthread_wait(NUM);                  \
      }                                           \
   }


inline void channel_in(channel * chan, void * message, int length) {
   sched.current[THREAD_SELF->id].c->chan_length = length;
   sched.current[THREAD_SELF->id].c->pointer = message;
   spinlock(&chan->clock);
   if (chan->communicator) {
      cthread * ctemp = chan->communicator;
      spinrelease(&chan->clock);
      chan->communicator = NULL;
      messcpy((char *) sched.current[THREAD_SELF->id].c->pointer,
              (char *) ctemp->pointer, 
           MIN(sched.current[THREAD_SELF->id].c->chan_length,ctemp->chan_length));
      // re_insert thread onto run queue
      scheduler_in(ctemp);
   } else {
      switch(THREAD_SELF->id) {
      case 0: comms_yield(0); break;
      case 1: comms_yield(1); break;
      case 2: comms_yield(2); break;
      case 3: comms_yield(3); break;
      }
    } // endif chan->communicator
}

// this out does not support alt
inline void channel_out(channel * chan, void * message, int length) {
   // if channel already exists just pass message
   sched.current[THREAD_SELF->id].c->chan_length = length;
   sched.current[THREAD_SELF->id].c->pointer = message;
   spinlock(&chan->clock);
   if (chan->communicator) {
      cthread * ctemp = chan->communicator;
      spinrelease(&chan->clock);
      chan->communicator = NULL;
      messcpy((char *) ctemp->pointer, 
              (char *) sched.current[THREAD_SELF->id].c->pointer,
           MIN(sched.current[THREAD_SELF->id].c->chan_length,ctemp->chan_length));
      scheduler_in(ctemp);
   } else {
      switch(THREAD_SELF->id) {
      case 0: comms_yield(0); break;
      case 1: comms_yield(1); break;
      case 2: comms_yield(2); break;
      case 3: comms_yield(3); break;
      }
   } // endif chan->communicator
}

#endif // LOCKFREE
