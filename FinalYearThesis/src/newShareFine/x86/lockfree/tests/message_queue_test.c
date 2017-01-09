#include "../message_queue.h"
#include <stdio.h>
struct lockfree_message_queue q;
int sum;
void t1(cthread * ct)
{
  int count;
  printf("Thread %d is sending messages\n", ct->id);
  for(count=1;count<=10;count++) {
    send_message((void*)count,&q);
    printf("Thread %d:message sent\n",ct->id);
  }
  printf("Thread %d has finished\n", ct->id);
  cthread_stop();
}

void t2(cthread *ct)
{
  int count;
  for(count=0;count<100;count++) {
    sum+=(int)fetch_message(&q);
    printf("Thread %d:got message\n",ct->id);
  }
  cthread_stop();
}

void cthread_main() {
  int count;
  cthread * c[11];
  message_queue_init(&q);
  for(count=0;count<11;count++) {
    if(count == 0) 
      c[count] = cthread_init(t2,409600,0);
    else
      c[count] = cthread_init(t1,409600,0);
    cthread_run(c[count]);
  }
  for(count=0;count<11;count++)
    cthread_join(c[count]);
  printf("The total sum is %d\n",sum);
}
