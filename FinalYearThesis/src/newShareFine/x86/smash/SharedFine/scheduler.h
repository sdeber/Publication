#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "cthread.h"
#include "kernelthread.h"

typedef struct {
   cthread * c;
   // cache line buffer good improvments
   unsigned char gap[32];
} cthold;

typedef struct {
   node * head;
   unsigned char gap1[32];
   node * tail;
   unsigned char gap2[32];
#ifdef TWOLOCK
   int * headlock;
   unsigned char gap3[32];
   int * taillock;
   unsigned char gap4[32];
#else
   int * qlock;
   unsigned char gap3[32];
#endif
   cthold current[PROCNUM]; 
      // the current running thread - independent from
      // the runqueue
   int sem_id[PROCNUM]; // semaphore id for kernel threads
   int main_sem_id; // semaphore id for main kernel thread
   volatile int ktready[PROCNUM];  // ready for kernel threads
   kernelthread * kt[PROCNUM]; // kernel thread info
   char * newstack[PROCNUM]; // used to save stack
   JmpBuf jmp[PROCNUM];
   int sleepnum;
   int threadnum;
   int schedlock;
} scheduler;

// this is required cause
// if we use a local variable this will be
// lost cause of stack change
cthread * ctemp[PROCNUM];

extern scheduler sched;

extern void scheduler_init(); 
extern void scheduler_in(cthread * c); 
extern void scheduler_schedule(int index);


// change to scheduler stack
#define scheduler_jmp2stack(INDEX) \
    asm volatile ("movl (%0),%%esp;movl 4(%0),%%ebp" : : "b" (&sched.jmp[INDEX]));

// this function only changes pc
#define scheduler_jump(INDEX) \
asm volatile ("movl $1,%%eax;jmp *8(%0)" : : "b" (&sched.current[INDEX].c->jmp) : /*"eax","ebx",*/"ecx","edx","esi","edi","cc","memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");


#define sched_jmp(INDEX) \
asm volatile ("movl $1,%%eax; movl (%0),%%esp\n\t;movl 4(%0),%%ebp\n\t;jmp *8(%0)" : : "b" (&sched.current[INDEX].c->jmp) : /*"eax","ebx",*/"ecx","edx","esi","edi","cc","memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");


// differentiating between the head and tail locks
// for the sched_enqueue() and sched_dequeue() functions
#ifdef TWOLOCK
// seperate locks for enqueue and dequeue
#define HEADLOCK sched.headlock
#define TAILLOCK sched.taillock
#else 
// head and tail locks are the same
// since we use the same locks for enqueueing
// and dequeueing
#define HEADLOCK sched.qlock
#define TAILLOCK sched.qlock
#endif // TWOLOCK

#define sched_enqueue(C)            \
{                                   \
   node * n = C->nodeptr;           \
   n->value = C;                    \
   n->next = NULL;                  \
   spinlock(TAILLOCK);              \
   sched.tail->next = n;            \
   sched.tail = n;                  \
   spinrelease(TAILLOCK);           \
}

#define sched_dequeue()             \
({                                  \
   node * head;                     \
   node * new_head;                 \
   cthread * value;                 \
   spinlock(HEADLOCK);              \
   head = sched.head;               \
   new_head = head->next;           \
   if (new_head == NULL) {          \
      spinrelease(HEADLOCK);        \
      value = NULL;                 \
   } else {                         \
      value = new_head->value;      \
      sched.head = new_head;        \
      spinrelease(HEADLOCK);        \
      value->nodeptr = head;        \
   }                                \
   value;                           \
})

#endif
