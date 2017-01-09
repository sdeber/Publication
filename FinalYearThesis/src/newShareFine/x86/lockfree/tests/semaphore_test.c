#include "../semaphore.h"
struct c_semaphore s;
void t1(cthread * ct)
{
  int count,ticket;
  printf("Thread %d is trying to get the semaphore\n", ct->id);
  ticket=csem_wait(&s);
  printf("Thread %d is in the critical session with ticket %d\n",ct->id,ticket);
  for(count=0;count<5;count++) {
  sleep(1);
  printf("Thread %d gives the CPU to others\n", ct->id);
  cthread_yield();
  printf("Thread %d is back now\n", ct->id); 
  }
  printf("Thread %d is releasing the semaphore\n", ct->id);
  ticket=csem_signal(&s);
  printf("Thread %d has released the semaphore with ticket %d\n", ct->id,ticket);
  cthread_stop();
}

void t2(cthread *ct)
{
  int count;
  for(count=0;count<2;count++) {
    printf("I am doing nothing\n");
    sleep(1);
  }
  cthread_stop();
}

void cthread_main() {
  int count;
  cthread * c[20];
  csem_init(&s,3);
  for(count=0;count<10;count++) {
    c[count] = cthread_init(t1,409600,0);
    cthread_run(c[count]);
 }
  for(;count<20;count++) {
    c[count] = cthread_init(t2,409600,0);
    cthread_run(c[count]);
   }
  for(count=0;count<20;count++)
    cthread_join(c[count]);
}
