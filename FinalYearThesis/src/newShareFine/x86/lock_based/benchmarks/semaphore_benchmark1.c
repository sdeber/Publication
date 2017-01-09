#include "../semaphore.h"
#include <sys/time.h>
#define THREAD_NUM 1
#define NUM 1000000
double calculate (struct timeval t1, struct timeval t2)
{
return ((double)(t2.tv_sec - t1.tv_sec))*1000000 + (t2.tv_usec - t1.tv_usec);
}
lockbased_csem s;
void t1(cthread * ct)
{
  int count,ticket;
  struct timeval t1,t2;
  gettimeofday(&t1,NULL);
  for(count=0;count<NUM;count++) {
	  lockbased_csem_wait(&s);
	  lockbased_csem_signal(&s);
  }
  gettimeofday(&t2,NULL);
  printf("Total time cost: %f\nPer operation: %f\n", calculate(t1,t2),calculate(t1,t2)/NUM);
  cthread_stop();
}

void cthread_main() {
  int count;
  cthread * c[THREAD_NUM];
  lockbased_csem_init(&s,100);
  for(count=0;count<THREAD_NUM;count++) {
    c[count] = cthread_init(t1,409600,0);
    cthread_run(c[count]);
 }
  for(count=0;count<THREAD_NUM;count++)
    cthread_join(c[count]);
}
