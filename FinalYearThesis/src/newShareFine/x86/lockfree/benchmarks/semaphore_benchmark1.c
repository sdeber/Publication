#include "../semaphore.h"
#include <sys/times.h>
#define THREAD_NUM 1
#define NUM 1000000
double calculate (struct timeval t1, struct timeval t2)
{
return ((double)(t2.tv_sec - t1.tv_sec))*1000000 + (t2.tv_usec - t1.tv_usec);
}
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
  printf("Total time cost: %f\nPer operation: %f\n",calculate(t1,t2),calculate(t1,t2)/NUM);
  cthread_stop();
}

void cthread_main() {
  int count;
  cthread * c[THREAD_NUM];
  csem_init(&s,100);
  for(count=0;count<THREAD_NUM;count++) {
    c[count] = cthread_init(t1,4096,0);
    cthread_run(c[count]);
 }
  for(count=0;count<THREAD_NUM;count++)
    cthread_join(c[count]);
}
