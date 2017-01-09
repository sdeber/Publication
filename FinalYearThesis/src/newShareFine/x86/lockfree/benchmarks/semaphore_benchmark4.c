#include "../semaphore.h"
#include <sys/times.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#define NUM 1000000
#define _SEM 111112
double calculate (struct timeval t1, struct timeval t2)
{
return ((double)(t2.tv_sec - t1.tv_sec))*1000000 + (t2.tv_usec - t1.tv_usec);
}
struct c_semaphore s;
union semun
{
  int val;                           
  struct semid_ds *buf;              
  unsigned short int *array;                    
};
union semun arg;
int sys_sem;
struct sembuf lock[1]={{0,-1,0}};
struct sembuf unlock[1]={{0,1,0}};
void t1(cthread * ct)
{
  int count,ticket;
  struct timeval t1,t2;
  gettimeofday(&t1,NULL);
  for(count=0;count<NUM;count++) {
    semop(sys_sem,lock,1);
    semop(sys_sem,unlock,0);
  }
  gettimeofday(&t2,NULL);
  printf("Total time cost: %f \nPer operation: %f\n", calculate(t1,t2),calculate(t1,t2)/NUM);
  cthread_stop();
}

void cthread_main() {
  int count;
  cthread * c;
  sys_sem = semget(_SEM,0,0);
  c = cthread_init(t1,4096,0);
  cthread_run(c);
  cthread_join(c);
}
