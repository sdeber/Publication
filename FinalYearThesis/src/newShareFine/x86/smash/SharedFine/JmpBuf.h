#ifndef __JMPBUF_H
#define __JMPBUF_H

typedef struct _JmpBuf JmpBuf;


#if DONOTSAVEACTIVECONTEXT
struct _JmpBuf {
    long int* sp;       /* 00 */
    long int* bp;       /* 04 */
    long int* pc;       /* 08 */
    long int ax;        /* 12 */
    long int bx;        /* 16 */
    long int cx;        /* 20 */
    long int dx;        /* 24 */
    long int si;        /* 28 */
    long int di;        /* 32 */
    long long int st0;  /* 36 */
    long long int st1;  /* 44 */
    long long int st2;  /* 52 */
    long long int st3;  /* 60 */
    long long int st4;  /* 68 */
    long long int st5;  /* 76 */
    long long int st6;  /* 84 */
    long long int st7;  /* 92 */
    long long int st8;  /* 92 */
    long long int st9;  /* 92 */
    long long int st10;  /* 92 */
    long long int st11;  /* 92 */
    long long int st12;  /* 92 */
    long long int st13;  /* 92 */
    long long int st14;  /* 92 */
};
#else
struct _JmpBuf {
    long int* sp;
    long int* bp;
    long int* pc;
};
#endif



//#include "JmpBuf.i"
extern void JmpBuf_Jmp(JmpBuf* jmp_buf);
extern void JmpBuf_Assign(JmpBuf* me, JmpBuf* other);

#if DONOTSAVEACTIVECONTEXT

#define JmpBuf_Set(JMPBUF)\
({\
    int result;\
    int aresult;\
    JmpBuf* _the_jmp_buf = JMPBUF;\
    asm volatile ("fsave 36(%1)\n\t"\
                  "movl %%esp,(%1)\n\t"\
                  "movl %%ebp,4(%1)\n\t"\
                  "movl %%eax,12(%1)\n\t"\
                  "movl %%ebx,16(%1)\n\t"\
                  "movl %%ecx,20(%1)\n\t"\
                  "movl %%edx,24(%1)\n\t"\
                  "movl %%esi,28(%1)\n\t"\
                  "movl %%edi,32(%1)\n\t"\
                  "fwait\n\t"\
                  // this is the point of return after JmpBuf_Jmp
                  // returns 1 though instead of 0
                  "movl $0f,8(%1)\n\t"\
                  "movl $0,%0\n\t"\
                  "0:" : "=&a" (aresult) :\
                  "r" (_the_jmp_buf) :\
                  "eax","cc","memory"\
                  );\
    result = aresult;\
})

#else 
#define JmpBuf_Set(JMPBUF)\
({\
    int result;\
    int aresult;\
    JmpBuf* _the_jmp_buf = JMPBUF;\
    asm volatile ("movl %%esp,(%1)\n\t"\
                  "movl %%ebp,4(%1)\n\t"\
                  "movl $0f,8(%1)\n\t"\
                  "movl $0,%0\n\t"\
                  "0:" : "=&a" (aresult) :\
                  "r" (_the_jmp_buf) :\
                  /*"eax","ebx",*/"ecx","edx","esi","edi","cc","memory",\
                  "st", "st(1)", "st(2)", "st(3)", "st(4)",\
                  "st(5)", "st(6)", "st(7)"\
                  );\
    result = aresult;\
})
#endif



#endif
