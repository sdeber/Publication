#include "../mutex.h"
#include <sys/times.h>
#include <unistd.h>
#define NUM 10000000
#define THREAD_NUM 1
cmutex_t m;
double calculate (struct timeval t1, struct timeval t2)
{
return ((double)(t2.tv_sec - t1.tv_sec))*1000000 + (t2.tv_usec - t1.tv_usec);
}
void t1(cthread * ct)
{
  int count;
  clock_t start,end;
  struct tms tstart,tend;
  struct timeval t1,t2;
  int clock_ticks = sysconf(_SC_CLK_TCK);
  //start=times(&tstart);
  gettimeofday(&t1,NULL);
  for(count=0;count<NUM;count++) {
	   GetMutex(&m);
	   ReleaseMutex(&m);
  }
  gettimeofday(&t2,NULL);
  //end=times(&tend);
  printf("Thread %d:time cost:%f\n",ct->id,calculate(t1,t2));
  printf("Thread %d:time cost:%f\n",ct->id,calculate(t1,t2)/NUM);
  cthread_stop();
}


void cthread_main() {
  int count;
  cthread * c[THREAD_NUM];
  cmutex_init(&m);
  for(count=0;count<THREAD_NUM;count++) {
    c[count] = cthread_init(t1,409600,0);
    cthread_run(c[count]);
  }
  for(count=0;count<THREAD_NUM;count++)
    cthread_join(c[count]);
}
