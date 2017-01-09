#include "../message_queue.h"
#include <stdio.h>
#include <sys/time.h>
#define THREAD_NUM 1000
#define NUM 10000
#define calculate_time(t1,t2) ((double)t2.tv_sec+t2.tv_usec/1000000 - ((double)t1.tv_sec+t1.tv_usec/1000000))
struct lockfree_message_queue q;
int sum;
void t1(cthread * ct)
{
  int count;
  struct timeval t1,t2;
  gettimeofday(&t1,NULL);
  for(count=1;count<=NUM;count++) {
    send_message((void*)1,&q);
  }
  gettimeofday(&t2,NULL);
  printf("Enqueuer:%d finished. time cost:%f\n",ct->id,calculate_time(t1,t2));
  cthread_stop();
}

void t2(cthread *ct)
{
  int count;
  struct timeval t1,t2;
  gettimeofday(&t1,NULL);
  for(count=0;count<NUM*(THREAD_NUM-1);count++) {
    sum+=(int)fetch_message(&q);
  }
  gettimeofday(&t2,NULL);
  printf("Dequeuer:%d finished. time cost:%f\n",ct->id,calculate_time(t1,t2));
  cthread_stop();
}

void cthread_main() {
  int count;
  cthread * c[THREAD_NUM];
  message_queue_init(&q);
  for(count=0;count<THREAD_NUM;count++) {
    if(count == 0) 
      c[count] = cthread_init(t2,409600,0);
    else
      c[count] = cthread_init(t1,409600,0);
    cthread_run(c[count]);
  }
  for(count=0;count<THREAD_NUM;count++)
    cthread_join(c[count]);
  printf("The total sum is %d\n",sum);
}
