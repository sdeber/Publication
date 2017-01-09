#include "scheduler.h"
#include "cthread.h"
#include "kernelthread.h"
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <linux/sem.h>
#include <signal.h>

#ifndef __USE_GNU
	#define __USE_GNU 1
#endif
#include <sched.h>
#include <pthread.h>

__thread kernelthreadptr local_self;

// locks and spins until lock is acquired
inline void spin_lock(int *lock)
{
    asm volatile ("1:  lock\n\t"  // set up lock on bus
                  "    btsl $0,%0\n\t"    //save bit 0 in carry and set bit 0
                  "    jnc 2f\n\t"        // jmp if carry == 0
                  "    jmp 1b\n\t"        // jmp back to loop                
                  "2:            "        // continue
                  : : "m" (*lock)
                 );
}


// releases the lock
inline void spin_release(int *lock)
{
    asm volatile  ( "lock\n\t"        // set up lock on bus
                   "btrl $0,%0"      // reset bit 0
                   : : "m" (*lock)
                 );
}

//#define PID

// This is temp. - until we find a proper solution
inline int kernelthread_num() {
#ifdef PID
/*    int pid = getpid();
    int i;
   
    for (i = 0;i < PROCNUM;i++) {
       if (sched.pid[i] == pid) return i;
    }
    printf("pid not found\n");
    return -1;*/
#else
    kernelthread * self = THREAD_SELF; 
    return self->id;
#endif
}


// kernel thread main functions
int kernelthread_start(void * index) {
	unsigned int * params = (unsigned int *) index;
    int thread_id = * (unsigned int *) params[0];
	pthread_t pthread_id = * (pthread_t *)params[1];
		
    kernelthread * self;

    fprintf(stderr,"Starting kernel thread %d\n",
	   thread_id);

    sched.kt[thread_id] = kt_init(thread_id, pthread_id);
    
    int cpu_mask = 1 << thread_id;

    if (sched_setaffinity(0, sizeof(int), &cpu_mask) < 0)
    {
    	perror("sched affinity failed");
    	abort();
    }
  
    // indicate thread is ready used for startup 
    sched.ktready[thread_id] = 1;   
    kernelthread_wait(thread_id);
    return 0;
}

// initiate sleeping structures
void kernelthread_init_sleep() {
   int counter;   
   union semun sema_semun;
    
   // For or the kernel threads 
   for (counter = 0; counter < PROCNUM; counter++) {
      sched.sem_id[counter] = 
             semget(IPC_PRIVATE,1,S_IRUSR | S_IWUSR | IPC_CREAT);
      sema_semun.val = 0;
      semctl(sched.sem_id[counter], 0, SETVAL, sema_semun );
   }

   // for main thread
   sched.main_sem_id = semget(IPC_PRIVATE,1,S_IRUSR | S_IWUSR | IPC_CREAT);
   sema_semun.val = 0;
   semctl(sched.main_sem_id, 0, SETVAL, sema_semun );
}


// remove sleeping structures
void kernelthread_dest_sleep() {
   int counter;   
   union semun sema_semun;
    
   // For or the kernel threads 
   for (counter = 0; counter < PROCNUM; counter++) {
      sema_semun.val = 0;
      semctl(sched.sem_id[counter], 0, IPC_RMID, sema_semun );
   }

   // for main thread
   sema_semun.val = 0;
   semctl(sched.main_sem_id, 0, IPC_RMID, sema_semun );
}

void kernelthread_kill(int signal) {
   int index = kernelthread_num();
   // kill semaphore 
   fprintf(stderr,"Shutting down kernel thread %d\n",index);
   exit(0);
}


kernelthread * kt_init(int id, pthread_t pthread_id) {
  kernelthread * kt = (kernelthread *) malloc(sizeof(kernelthread));
  kt->pid = pthread_id;
  kt->id = id;
  kt->self = kt;
  INIT_THREAD_SELF(kt);
  return kt;
}

void pthread_terminate(int signal)
{
	pthread_exit(NULL);
}

// initialise kernel thread routines
void kernelthread_init() {
      //void *stack[PROCNUM];
      int counter;
      unsigned int *index[PROCNUM];
    
     for (counter = 0; counter < PROCNUM; counter++) {
       //stack[counter] = (void *) (((char *) malloc(THREADSTACKSIZE)) + THREADSTACKSIZE);
       index[counter] = (unsigned int *) malloc(sizeof(unsigned int));
       *index[counter] = counter;
       // start up the other kernel threads adn initialise their pid
       //clone(&kernelthread_start,stack[counter],CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND,index[counter]);
       pthread_t* pthread = malloc(sizeof (pthread_t));
       unsigned int * param = malloc(sizeof(unsigned int) * 2);
       param[0] = index[counter]; 
       param[1] = pthread;
       pthread_create(pthread, NULL, (void *) &kernelthread_start, param);
     }
   // set signal handler for kernel threads
     signal(SIGUSR1,pthread_terminate);
}


// function used to launch main and
// so chtread_main has something to return to
// instead of using cthread_stop - it's more efficient
// to shut down everything from here
int launch_main() {
   static struct sembuf semosleep[1]={{0,1,0}};
   cthread_main();
   // wake up main thread to shut all the shit down
   semop(sched.main_sem_id, semosleep,1);
}

int main() {
   int i;
   cthread * ctmain; //the main thread
   kernelthread * ktmain; // this is the main kernel thread 
                           // not used for much except to init
                           // the ldt 
   kernelthread * self; 
   static struct sembuf semosleep[1]={{0,-1,0}};


   ktmain = kt_init(-1, getpid());
   scheduler_init();
   kernelthread_init_sleep();
   kernelthread_init();
   ctmain = cthread_init(launch_main,1500000,0);

   // this is used to make sure kernel threads are running
   // before starting our scheduler

   for (i = 0; i < PROCNUM; i++)
   while(!sched.ktready[i]);

   fprintf(stderr,"Launching cthread_main\n");

   // insert main thread on queue and jump to it
   // notice no need to save context here!

   scheduler_in(ctmain);
   
   fprintf(stderr,"Main thread is dozing off\n");
   semop(sched.main_sem_id, semosleep,1);
 
   // we get here when the main user thread finishes
   fprintf(stderr,"Main thread has finished - shutting down\n");
   fprintf(stderr,"Send signal to kill all child process\n");
    
   for (i = 0; i < PROCNUM;i++) {
//      kill(sched.kt[i]->pid,SIGUSR1);
      //kill(sched.kt[i]->pid,SIGINT);
	   if (pthread_kill(sched.kt[i]->pid,SIGUSR1) < 0)
		   perror("pthread_kill()");
   }
   // remove main semaphore
   kernelthread_dest_sleep();
   
   fprintf(stderr,"Sempahores deleted, exiting...\n");
 
   return 0;
}



