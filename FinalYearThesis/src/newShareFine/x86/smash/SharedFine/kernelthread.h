#ifndef __KERNELTHREAD_H
#define __KERNELTHREAD_H

#define THREADSTACKSIZE 2500000

#include <asm/ldt.h>
#include <pthread.h>

typedef struct kernelthread_struct * kernelthreadptr;

struct kernelthread_struct{
   kernelthreadptr self;       // Pointer to this structure
   int id;
   pthread_t pid;
};

typedef struct kernelthread_struct  kernelthread;

extern __thread kernelthreadptr local_self;

#define THREAD_SELF \
({           		\
	local_self;		\
})

#define INIT_THREAD_SELF(descr) 	\
{									\
    local_self = descr;				\
}

extern int kernelthread_num(); 
int kernelthread_start(void * index); 
kernelthread * kt_init(int i, pthread_t pthread_id);
void kernelthread_init(); 
void kernelthread_wait(int index);
void set_kernelthread(int num);
int get_kernelthread();
extern void spin_lock(int *lock);
extern void spin_release(int *lock);

#endif


