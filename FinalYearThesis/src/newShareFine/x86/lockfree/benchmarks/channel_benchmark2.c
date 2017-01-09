#include "../channel.h"
#include "../../smash/SharedFine/smash.h"
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#define CHAN_NUM 4000

double calculate (struct timeval t1, struct timeval t2)
{
return ((double)(t2.tv_sec - t1.tv_sec))*1000000 + (t2.tv_usec - t1.tv_usec);
}
channel * chan_array[CHAN_NUM];
void chan_in(cthread * ct)
{
  int count;
  char message;
  for(count = 0; count < CHAN_NUM; count++) {
    channel_in_alt(chan_array,CHAN_NUM,&message,1);
  }
  cthread_stop();
}

void chan_out(cthread * ct,int n)
{
  char message;
  //printf("thead:%d\n start outputting on Channel %d\n", ct->id,n);
  channel_out(chan_array[n], &message, 1);
  //printf("thead:%d\n finish outputting\n", ct->id);
  cthread_stop();
}
void cthread_main() {
  int i;
  struct timeval t1,t2;
  cthread * c[CHAN_NUM+1];
  for(i=0; i < CHAN_NUM; i++)
    chan_array[i]=(channel*)malloc(sizeof(channel));
  for(i=0; i < CHAN_NUM; i++)
    chan_array[i]->communicator=NULL;
  for(i =0; i < CHAN_NUM + 1; i++)
    if(i==0)
      c[i] = cthread_init(chan_in,40960,0);
    else
      c[i] =  cthread_init(chan_out,4096,1,i-1);
  gettimeofday(&t1,NULL);
  for(i=0; i < CHAN_NUM+1; i++){
    cthread_run(c[i]);
  }
  for(i=0;i<CHAN_NUM+1;i++)
    cthread_join(c[i]);
  gettimeofday(&t2,NULL);
  printf("Total time cost:%f\n",calculate(t1,t2));
}
