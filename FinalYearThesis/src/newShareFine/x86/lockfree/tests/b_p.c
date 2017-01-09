#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
#define THREAD_NUM 100
#define NUM 100
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
void *test(void * a)
{
	struct timeval t1,t2;
	int counter;
	gettimeofday(&t1,NULL);
	for(counter=0;counter<NUM;counter++){
		    pthread_mutex_lock(&mtx);
		        pthread_mutex_unlock(&mtx);
			    }	
	gettimeofday(&t2,NULL);
	printf("Time :%f\n",(double)t2.tv_sec+(t2.tv_usec)/1000000 - ((double)t1.tv_sec + (t1.tv_usec)/1000000));
}
int main()
{
	int counter;
	pthread_t tids[THREAD_NUM];
	for(counter=0;counter<THREAD_NUM;counter++){
	pthread_create(&tids[counter],NULL,test,NULL);}
	for(counter=0;counter<THREAD_NUM;counter++)
	pthread_join(tids[counter],NULL);
}
