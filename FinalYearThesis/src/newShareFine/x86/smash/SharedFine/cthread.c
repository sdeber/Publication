#include "cthread.h"
#include "scheduler.h"
#include "JmpBuf.i"
#include "kernelthread.h"
#include <stdarg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <sched.h>
#include <sys/stat.h>
#include <signal.h>


#include "scheduler.i"
#include "kernelthread.i"


typedef unsigned long int32;

// lock used for malloc
int malloclock = 0;

inline void cthread_stop() {
   // if joiner is NULL nothing to do plus set joiner
   // so that join will know that we touched the pointer
   int index = THREAD_SELF->id;
   int * clock = &sched.current[index].c->clock;

   spinlock(clock);
   if (sched.current[index].c->joiner == NULL) {
       sched.current[index].c->joiner = sched.current[index].c;
       spinrelease(clock);
       // no need to join
       scheduler_jmp2stack(index);
       index = THREAD_SELF->id;

       if (sched.current[index].c = sched_dequeue()) {
          sched_jmp(index);
       } else {
          spinlock(&sched.schedlock);
          sched.sleepnum++;
          spinrelease(&sched.schedlock);
          kernelthread_wait(index);
       }
   } else { //if sched.current[index].c->joiner == NULL
      // else we run the joiner thread
      cthread * tempcurrent;
      spinrelease(clock);
      scheduler_jmp2stack(index);
      index = THREAD_SELF->id;
      tempcurrent = sched.current[index].c;
      sched.current[index].c = tempcurrent->joiner;
      sched_jmp(index);
   }
}

node * node_init(cthread * d) {
   node * n;
   spinlock(&malloclock);
   n = (node *) malloc(sizeof(node));
//   n = (node *) malloc(sizeof(node) + L1_CACHE_BYTES);
  // n = (node *) L1_CACHE_ALIGN(n);
   spinrelease(&malloclock);
   n->value = d;
   n->next = NULL;
   return n;
}


inline cthread * cthread_init_extra(Funcptr funcptr,int stacksize,
                       int narguments,...) {
    cthread * c;
    long int* start_bp;
    long int* start_sp;
    int result;
    int i;
    va_list l; // va_list is a standard type - see <stdarg.h>

    
    spinlock(&malloclock);
    c = (cthread *) malloc( sizeof (cthread) + L1_CACHE_BYTES);
    c = (cthread *) L1_CACHE_ALIGN(c);
    c->stack = (char *) malloc(stacksize); 
    spinrelease(&malloclock);
    c->clock = 0;
    c->nodeptr = node_init(c);
    c->id = sched.threadnum++;
    c->funcptr = funcptr;
//    c->status = NOTRUNNING;
    c->joiner = NULL;
    c->temp=NULL;
    start_bp = ((long int*)c->stack)+(stacksize>>2)-1;
    start_sp = start_bp-narguments-1-40;
    va_start(l,narguments);
    // set start of stack to point to thread
    // if we want to make the thread the 1st argument
    // this is not however necessairy
    start_sp[0] = (long int)c;
    // all the arguments are placed onto the stack
    for(i=1;i<=narguments;i++) {
        start_sp[i] = va_arg(l,int);
    }
    va_end(l);
    asm volatile ("movl %%esp,(%1)\n\t"
                  "movl %%ebp,4(%1)\n\t"
                  "movl $0f,8(%1)\n\t"
                  "movl $0,%0\n\t"
                  "0:" : "=&a" (result) :
                  "r" (&c->initial_jmp) :
                  /*"eax","ebx",*/"ecx","edx","esi","edi","cc","memory",
                  "st", "st(1)", "st(2)", "st(3)", "st(4)",
                  "st(5)", "st(6)", "st(7)"
                  );

    if (result) {
        asm volatile ("call *%0\n\t"
                      "call cthread_stop ": :
                      "r" (sched.current[THREAD_SELF->id].c->funcptr) :
                      /*"eax","ebx",*/"ecx","edx","esi","edi","cc","memory",
                      "st", "st(1)", "st(2)", "st(3)", "st(4)",
                      "st(5)", "st(6)", "st(7)"
                      );
    }
    // Assign stack pointers to jump buffer
    c->initial_jmp.sp = start_sp;
    c->initial_jmp.bp = start_bp;
    // Copy initial jump buffer to jump buffer
    JmpBuf_Assign(&c->jmp,&c->initial_jmp);
    return c;
}


// this does the same thing
// for each yield function
#define yield_macro(NUM)                             \
      if (!JmpBuf_Set(&sched.current[NUM].c->jmp)) {   \
         if (ctemp[NUM] = sched_dequeue()) {         \
            cthread_jmp2stack(NUM);                  \
            sched_enqueue(sched.current[NUM].c);       \
            sched.current[NUM].c = ctemp[NUM];         \
         }                                           \
         sched_jmp(NUM);                             \
      }


// this is funny, it works - faster when inline
// however locks still fuck up overall performance - so doesn't matter
// otherwise i think optimization makes it slower :(
// we never set index - cause it costs ca. 10 ns per access
// so we use a switch or conditional
// note: architecture dependant (in the PROCNUM sense)
inline void cthread_yield() {
   switch (THREAD_SELF->id) {
      case 0: yield_macro(0); break;
      case 1: yield_macro(1); break;
      case 2: yield_macro(2); break;
      case 3: yield_macro(3); break;
   }
}





inline void cthread_join(cthread * c) {
  int index = THREAD_SELF->id;
    if (!JmpBuf_Set(&sched.current[index].c->jmp)) {
       ctemp[index] = c;
       scheduler_jmp2stack(index);
       index = THREAD_SELF->id;
       c = ctemp[index];

       // if joiner is NULL we set joiner
       // else we do nothing
       spinlock(&c->clock);
       if (c->joiner == NULL) { 
          c->joiner = sched.current[index].c;
	  spinrelease(&c->clock);
          // joiner has been set jump to it
          if (sched.current[index].c = sched_dequeue()) {
             scheduler_schedule(index);
          } else {
             spinlock(&sched.schedlock);
             sched.sleepnum++;
             spinrelease(&sched.schedlock);
             kernelthread_wait(index); 
          } 
       } else { // if c->joiner == NULL
           //nothing to do joiner already exists
           spinrelease(&c->clock);
           sched_jmp(index);
       }
    }
}


