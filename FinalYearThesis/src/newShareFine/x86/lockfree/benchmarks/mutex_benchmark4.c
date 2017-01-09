#include "../mutex.h"
#include <sys/times.h>
#include <unistd.h>
#define NUM 10
#define THREAD_NUM 200000
cmutex_t m;
void test(cthread * ct)
{
  int count;
  struct timeval t1,t2;
  gettimeofday(&t1,NULL);
  for(count=0;count<NUM;count++) {
	   GetMutex(&m);
	   ReleaseMutex(&m);
  }
  gettimeofday(&t2,NULL);
  printf("Thread %d:time cost:%f\n",ct->id,(double)t2.tv_sec+t2.tv_usec/1000000 - (double)t1.tv_sec + t1.tv_usec/1000000);
  cthread_stop();
}


void cthread_main() {
  int count;
  struct timeval t1,t2;
  cthread * c[THREAD_NUM];
  cmutex_init(&m);
  gettimeofday(&t1,NULL);
  for(count=0;count<THREAD_NUM;count++) {
    c[count] = cthread_init(test,4096,0);
    cthread_run(c[count]);
  }
  for(count=0;count<THREAD_NUM;count++)
    cthread_join(c[count]);
  gettimeofday(&t2,NULL);
  printf("Main Thread:time cost:%f\n",(double)t2.tv_sec+t2.tv_usec/1000000 - (double)t1.tv_sec + t1.tv_usec/1000000);

}
