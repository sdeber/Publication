#include "../message_queue.h"
#include <stdio.h>
#include <sys/time.h>
#define NUM 50000

double calculate (struct timeval t1, struct timeval t2)
{
return ((double)(t2.tv_sec - t1.tv_sec))*1000000 + (t2.tv_usec - t1.tv_usec);
}

struct message_queue q;
void test(cthread *ct)
{
  int count;
  struct timeval t1,t2;
  for(count=0;count<NUM;count++) 
    send_message((void*)1,&q);
  gettimeofday(&t1,NULL);
  for(count=0;count<NUM;count++)
    fetch_message(&q);
  gettimeofday(&t2,NULL);
  printf("Total time cost:%f\nTime Per Operation:%f\n",calculate(t1,t2),calculate(t1,t2)/NUM);
  cthread_stop();
}

void cthread_main() {
  int count;
  cthread * c;
  message_queue_init(&q); 
  c = cthread_init(test,40960,0);
  cthread_run(c);
  cthread_join(c);
}
