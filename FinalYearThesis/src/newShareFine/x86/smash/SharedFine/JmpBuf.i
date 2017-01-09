#if DONOTSAVEACTIVECONTEXT
// the context switch !!!
inline void JmpBuf_Jmp(JmpBuf* jmp_buf)
     /* Jumps to the context in the JmpBuf.
      */
{
    // the numbers below ex. 4,12, etc. correspond to 
    // locations inside the jmp_buf structure; refer to JmpBuf.h 
    asm volatile ("movl (%0),%%esp\n\t"
		  "movl 4(%0),%%ebp\n\t"
#if 1
		  "frstor 36(%0)\n\t"
#endif
		  "movl 12(%0),%%eax\n\t"
		  "movl 16(%0),%%eax\n\t"
		  "movl 20(%0),%%ecx\n\t"
		  "movl 24(%0),%%edx\n\t"
		  "movl 28(%0),%%esi\n\t"
		  "movl 32(%0),%%edi\n\t"
		  "movl $1,%%eax\n\t"
                  // program counter below
		  "jmp *8(%0)" : :
		  "b" (jmp_buf) :
		  "eax","ebx","cc","memory"
		  );
}

// makes a copy of the context switch structure
// copies other on to me, i.e. me = other
inline void JmpBuf_Assign(JmpBuf* me, JmpBuf* other)
{
    me->sp = other->sp;
    me->bp = other->bp;
    me->pc = other->pc;
    me->ax = other->ax;
    me->bx = other->bx;
    me->cx = other->cx;
    me->dx = other->dx;
    me->si = other->si;
    me->di = other->di;
    me->st0 = other->st0;
    me->st1 = other->st1;
    me->st2 = other->st2;
    me->st3 = other->st3;
    me->st4 = other->st4;
    me->st5 = other->st5;
    me->st6 = other->st6;
    me->st7 = other->st7;
    me->st8 = other->st8;
    me->st9 = other->st9;
    me->st10 = other->st10;
    me->st11 = other->st11;
    me->st12 = other->st12;
    me->st13 = other->st13;
    me->st14 = other->st14;
}
#else
inline void JmpBuf_Jmp(JmpBuf* jmp_buf)
     /* Jumps to the context in the JmpBuf.
      */
{
    asm volatile ("movl (%0),%%esp\n\t"
		  "movl 4(%0),%%ebp\n\t"
		  "movl $1,%%eax\n\t"
		  "jmp *8(%0)" : :
		  "b" (jmp_buf) :
		  /*"eax","ebx",*/"ecx","edx","esi","edi","cc","memory",
		  "st", "st(1)", "st(2)", "st(3)", "st(4)",
		  "st(5)", "st(6)", "st(7)"
		  );
}

inline void JmpBuf_Assign(JmpBuf* me, JmpBuf* other)
{
    me->sp = other->sp;
    me->bp = other->bp;
    me->pc = other->pc;
}
#endif
