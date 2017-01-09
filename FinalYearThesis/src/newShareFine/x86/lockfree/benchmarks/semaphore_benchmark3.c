#include "../semaphore.h"
#include <sys/times.h>
#define TASK_NUM 6
#define GRANULARITY 2000000
#define THREAD_PER_TASK 10
double calculate (struct timeval t1, struct timeval t2)
{
return ((double)(t2.tv_sec - t1.tv_sec))*1000000 + (t2.tv_usec - t1.tv_usec);
}
struct c_semaphore s[TASK_NUM];
void test(cthread * ct, int task_num)
{
  int count,n=0;
  csem_wait(&s[task_num]);
  for(count=0;count< GRANULARITY;count++)
    n++;
  csem_signal(&s[task_num]);
  cthread_stop();
}

void cthread_main() {
  int count;
  int thread_num = THREAD_PER_TASK * TASK_NUM;
  int thread_count =0;
  struct timeval t1,t2;
  cthread * c[TASK_NUM][THREAD_PER_TASK];
  for(count=0;count<TASK_NUM;count++)
    csem_init(&s[count],1);
  for(count=0;count<TASK_NUM;count++)
    for(thread_count=0;thread_count<THREAD_PER_TASK;thread_count++)
      c[count][thread_count] = cthread_init(test,4096,1,count);
  gettimeofday(&t1,NULL);
  for(count=0;count<TASK_NUM;count++)
    for(thread_count=0;thread_count<THREAD_PER_TASK;thread_count++)
      cthread_run(c[count][thread_count]);
  for(count=0;count<TASK_NUM;count++)
    for(thread_count=0;thread_count<THREAD_PER_TASK;thread_count++)
      cthread_join(c[count][thread_count]);
  gettimeofday(&t2,NULL);
  printf("Main Thread:time cost:%f\n",calculate(t1,t2));
}
