#ifndef __CTHREAD_H
#define __CTHREAD_H

#include "global.h"
#include "JmpBuf.h"
#include <stdio.h>
#include <stdarg.h>

#define L1_CACHE_BYTES 32
#define L1_CACHE_ALIGN(x) (((unsigned long)x+31) & (~31))

typedef struct ct cthread;
typedef struct nodestruct node;
typedef void (*Funcptr) (cthread * c,...);


struct ct {
   JmpBuf jmp;
   struct ct * next;  
   char * stack;
   node * nodeptr;
   JmpBuf initial_jmp;
   Funcptr funcptr; // function to be run
   int id;
   int clock;           // thread lock structure for join
   void * pointer;     // channel buffer for message 
   int chan_length;    // lenght of message
   struct ct * joiner; // this must eventually be a list
                       // since we can have more than 1
                       // function call a thread to join with
  struct ct * temp;
};


struct nodestruct {
   cthread * value;
   node * next;
   unsigned char gap[32];
};

node * node_init(cthread * c);

#define cthread_init(F,X...) cthread_init_extra((Funcptr)(F),X)
extern cthread * cthread_init_extra(Funcptr funcptr, int stacksize,
                       int narguments,...);
extern void cthread_stop();
extern void cthread_run(cthread * c);
extern void cthread_join();
extern void cthread_main(); // main function for API - user must implement it
extern void cthread_yield();

// jump to next stack
#define cthread_jmp2stack(INDEX) \
asm volatile ( "movl (%0),%%esp;movl 4(%0),%%ebp\n\t" : : "b" (&ctemp[INDEX]->jmp));

#define spinlock(LOCK) \
   asm volatile ("1:  lock; btsl $0,%0;jnc 2f;jmp 1b; 2:  " : : "m" (*LOCK));

/*
#define spinlock(LOCK) \
  asm volatile ("1:  lock;btsl $0,%0;jnc 3f;2:  btl $0,%0;jc 2b;jmp 1b;3: " :: "m" (*LOCK));
*/

#define spinrelease(LOCK) \
    asm volatile ( "lock;btrl $0,%0" : : "m" (*LOCK));



#define cthread_run(CT)       \
{                             \
    scheduler_in(CT);         \
}


#endif
