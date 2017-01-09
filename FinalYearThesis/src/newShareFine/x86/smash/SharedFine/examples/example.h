// bloody lock is really expensive :(
#define slock(LOCK) \
   asm volatile ("1:  lock; btsl $0,%0;jnc 2f;jmp 1b; 2:  " : : "m" (*LOCK));

#define srelease(LOCK) \
    asm volatile ( " lock; btrl $0,%0" : : "m" (*LOCK));

// this is the version where we use a variable
// to maintain the identity of THREAD_SELF->id
// ca. 20% slower than the one below
inline void cthread_yield2() {
   int index = THREAD_SELF->id;
   if (!JmpBuf_Set(&sched.current[index]->jmp)) {
      if (ctemp[index] = sched_dequeue()) {
          index = THREAD_SELF->id;
          sched_enqueue(sched.current[index]);
          sched.current[index] = ctemp[index];
      }
      sched_jmp(index);
   }
}

/*
// this does the same thing
// for each yield function
#define yield_macro(NUM)                             \
      if (!JmpBuf_Set(&sched.current[NUM]->jmp)) {   \
         slock(sched.qlock);                         \
         if (ctemp[NUM] = sched_dequeue()) {         \
            cthread_jmp2stack(NUM);                  \
            sched_enqueue(sched.current[NUM]);       \
            sched.current[NUM] = ctemp[NUM];         \
         }                                           \
         srelease(sched.qlock)                       \
         sched_jmp(NUM);                             \
      }

// this is funny, it works - faster when inline
// however locks still fuck up overall performance - so doesn't matter
// otherwise i think optimization makes it slower :( 
// we never set index - cause it costs ca. 10 ns per access
// so we use a switch or conditional 
// note: architecture dependant (in the PROCNUM sense)
void cthread_yield2() {
   switch (THREAD_SELF->id) {
      case 0: yield_macro(0); break;
      case 1: yield_macro(1); break;
      case 2: yield_macro(2); break;
      case 3: yield_macro(3); break;
   }  
}
*/

/*
inline void cthread_yield2() {
   int index = THREAD_SELF->id;
   if (!JmpBuf_Set(&sched.current[index]->jmp)) {
       spinlock(sched.qlock);
       sched_enqueue(sched.current[index]);
       sched.current[index] = sched_dequeue();
       spinrelease(sched.qlock);
       sched_jmp(index);
   }
}
*/

/*
// version that does not change stack - but releases and jumps
inline void cthread_yield2() {
   int index = THREAD_SELF->id;
   spinlock(sched.qlock);
   if (!JmpBuf_Set(&sched.current[index]->jmp)) {
      if (ctemp[index] = sched_dequeue()) {
          sched_enqueue(sched.current[index]);
          sched.current[index] = ctemp[index];
      }
     // release and jump
        asm volatile (
                  "movl (%0),%%esp\n\t"
                  "movl 4(%0),%%ebp\n\t"
                  "lock\n\t"        // set up lock on bus
                  "btrl $0,%1\n\t"      // reset bit 0
                  "jmp *8(%0)" : :
                  "b" (&sched.current[index]->jmp) ,
                  "m" (*sched.qlock) :
                  "eax","ebx","ecx","edx","esi","edi","cc","memory",
                  "st", "st(1)", "st(2)", "st(3)", "st(4)",
                  "st(5)", "st(6)", "st(7)"
                  );
       }
}
*/

