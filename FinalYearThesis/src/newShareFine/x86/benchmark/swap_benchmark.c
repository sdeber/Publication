#include <stdio.h>
#include "../primitives.c"
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
#define calculate(start,end) ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec))
long a;
void *test(void * a)
{
  struct timeval t1,t2;
  int counter;
  double diff=0;
  gettimeofday(&t1,NULL);
  for(counter=0;counter<10000000;counter++){
    FetchAndAdd(&a,1);
  }
  gettimeofday(&t2,NULL);
  diff = calculate(t1,t2);
  printf("Total Time :%f Microseconds\nPer operation time:%f Microseconds\n",diff,diff / 10000000);
}
int main()
{
  pthread_t tid;
  pthread_create(&tid,NULL,test,NULL);
  pthread_join(tid,NULL);
}
