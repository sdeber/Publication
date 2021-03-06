#include "../channel.h"
#include "../../smash/SharedFine/smash.h"
#include <stdlib.h>
#include <stdio.h>
channel * chan_array[10];

void chan_in(cthread * ct)
{
int count;
char message[10];
for(count = 0; count < 10; count++)
{
  //printf("Thread:%d starts inputting\n",ct->id);
channel_in_alt(chan_array,10,message,10);
/*int i;
for(i=0;i<10;i++) {
printf("%d",message[i]);
printf("\n");
}*/
}
cthread_stop();
}

void chan_out(cthread * ct,int n)
{
char message[10];
int count;
for(count =0; count <10;count++)
message[count] = 48+n;
//printf("thead:%d\n start outputting on Channel %d\n", ct->id,n);
channel_out(chan_array[n], message, 10);
//printf("thead:%d\n finish outputting\n", ct->id);
cthread_stop();
}
void cthread_main() {
int i =11;
cthread * c[11];
for(i=0;i<10;i++)
chan_array[i]=(channel*)malloc(sizeof(channel));
for(i=0;i<10;i++)
chan_array[i]->communicator=NULL;
for(i =0;i<11;i++)
if(i==0)
c[i] = cthread_init(chan_in,40960,0);
else
c[i] =  cthread_init(chan_out,40960,1,i-1);
for(i=0;i<11;i++){
cthread_run(c[i]);
}
for(i=0;i<11;i++)
cthread_join(c[i]);
}
