#include "../mutex.h"
#include <sys/times.h>
#include <unistd.h>
#define TASK_NUM 10
#define GRANULARITY 100000
#define THREAD_PER_TASK 100
double calculate (struct timeval t1, struct timeval t2)
{
return ((double)(t2.tv_sec - t1.tv_sec))*1000000 + (t2.tv_usec - t1.tv_usec);
}
cmutex_t m[TASK_NUM];
long task[TASK_NUM];
void test(cthread * ct,int task_num)
{
  int count,n=0;
  GetMutex(&m[task_num]);
  for(count=0;count < GRANULARITY;count++)
    n++;
  ReleaseMutex(&m[task_num]);
  cthread_stop();
}

void cthread_main() {
  int count,thread_num = THREAD_PER_TASK * TASK_NUM;
  int thread_count = 0;
  struct timeval t1,t2;
  cthread * c[TASK_NUM][THREAD_PER_TASK];
  for(count=0;count<TASK_NUM;count++)
    cmutex_init(&m[count]);
  gettimeofday(&t1,NULL);
  for(count=0;count<TASK_NUM;count++)
    for(thread_count=0;thread_count<THREAD_PER_TASK;thread_count++)
      c[count][thread_count] = cthread_init(test,4096,1,count);

  for(count=0;count<thread_num;count++) 
  cthread_run(c[count]);
  for(count=0;count<thread_num;count++)
    cthread_join(c[count]);
  gettimeofday(&t2,NULL);
  printf("Main Thread:time cost:%f\n",calculate(t1,t2));

}
