#include "../mutex.h"
cmutex_t m;
void t1(cthread * ct)
{
  int count;
  printf("Thread %d is trying to get the mutex\n", ct->id);
  GetMutex(&m);
  printf("Thread %d is in the critical session\n",ct->id);
  for(count=0;count<5;count++) {
  sleep(1);
  printf("Thread %d gives the CPU to others\n", ct->id);
  cthread_yield();
  printf("Thread %d is back now\n", ct->id); 
  }
  printf("Thread %d is releasing the mutex\n", ct->id);
  ReleaseMutex(&m);
  printf("Thread %d has released the mutex\n", ct->id);
  cthread_stop();
}

void t2(cthread *ct)
{
  int count;
  for(count=0;count<10;count++) {
    printf("I am doing nothing\n");
    sleep(1);
  }
  cthread_stop();
}

void cthread_main() {
  int count;
  cthread * c[10];
  cmutex_init(&m);
  for(count=0;count<5;count++) {
    c[count] = cthread_init(t1,409600,0);
    cthread_run(c[count]);
  }
  for(;count<10;count++) {
    c[count] = cthread_init(t2,409600,0);
    cthread_run(c[count]);
  }
  for(count=0;count<10;count++)
    cthread_join(c[count]);
}
