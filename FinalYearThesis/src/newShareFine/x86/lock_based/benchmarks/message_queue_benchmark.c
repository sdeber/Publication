#include "../message_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define THREAD_NUM 25000
#define NUM 1
double calculate (struct timeval t1, struct timeval t2)
{
return ((double)(t2.tv_sec - t1.tv_sec))*1000000 + (t2.tv_usec - t1.tv_usec);
}
struct message_queue q;
void test(cthread * ct)
{
  int count;
  for(count=1;count<=NUM;count++) {
    send_message((void*)1,&q);
  }
  cthread_stop();
}

void cthread_main() {
  int count;
  struct timeval t1,t2;
  cthread * c[THREAD_NUM];
  message_queue_init(&q);
  for(count=0;count<THREAD_NUM;count++)
    c[count] = cthread_init(test,4096,0);
  gettimeofday(&t1,NULL);
  for(count=0;count<THREAD_NUM;count++)
     cthread_run(c[count]);
  for(count=0;count<THREAD_NUM;count++)
    cthread_join(c[count]);
  gettimeofday(&t2,NULL);
  printf("time cost:%f\nPer operation:%f\n",calculate(t1,t2),calculate(t1,t2)/THREAD_NUM);
}
