#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
void *test(void * a)
{
struct timeval t1,t2;
int counter;
gettimeofday(&t1,NULL);
for(counter=0;counter<10000000;counter++){
    pthread_mutex_lock(&mtx);
    pthread_mutex_unlock(&mtx);
    }	
gettimeofday(&t2,NULL);
printf("Time :%f\n",(((double)t2.tv_sec-(double)t1.tv_sec)*1000000+(t2.tv_usec - t1.tv_usec))/10000000);
}
int main()
{
pthread_t tid;
pthread_create(&tid,NULL,test,NULL);
pthread_join(tid,NULL);
}
