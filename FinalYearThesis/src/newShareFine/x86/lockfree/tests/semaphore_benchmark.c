#include "../semaphore.h"
#include <sys/times.h>
#define THREAD_NUM 1000
#define NUM 1000000
#define calculate_time(t1,t2) ((double)t2.tv_sec+t2.tv_usec/1000000 - ((double)t1.tv_sec+t1.tv_usec/1000000))
struct c_semaphore s;
void t1(cthread * ct)
{
  int count,ticket;
  struct timeval t1,t2;
  gettimeofday(&t1,NULL);
  for(count=0;count<NUM;count++) {
	  csem_wait(&s);
	  csem_signal(&s);
  }
  gettimeofday(&t2,NULL);
  printf("Thread %d: time cost %f \n", ct->id,calculate_time(t1,t2));
  cthread_stop();
}

void cthread_main() {
  int count;
  cthread * c[THREAD_NUM];
  csem_init(&s,100);
  for(count=0;count<THREAD_NUM;count++) {
    c[count] = cthread_init(t1,409600,0);
    cthread_run(c[count]);
 }
  for(count=0;count<THREAD_NUM;count++)
    cthread_join(c[count]);
}
