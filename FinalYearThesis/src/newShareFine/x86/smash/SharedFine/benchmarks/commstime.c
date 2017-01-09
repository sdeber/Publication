/* 
 * commstime for smash - adapted from MESH commstime
 */

#include <stdio.h>
#include "smash.h"
#include "channel.h"
#include <sys/time.h>


float calctime(struct timeval * t2, struct timeval *t1) {
   int temp = t2->tv_sec - t1->tv_sec;
   int temp2 = t2->tv_usec - t1->tv_usec;
   return ((temp * 1000000) + temp2);
}


void Prefix (cthread *p, int n, channel *in, channel *out) {
  int value;
  channel_out(out,(void *) &n, sizeof(int));
  while (1) {
     channel_in(in,(void *) &value, sizeof(int));
     channel_out(out,(void *) &value, sizeof(int));
  }
}

void Delta (cthread *p, channel *in, channel *out_1, channel *out_2) {
  int value;
  while (1) {
    channel_in(in,(void *) &value, sizeof(int)); 
    channel_out(out_1,(void *) &value, sizeof(int));
    channel_out(out_2,(void *) &value, sizeof(int)); 
  }
}

void Succ (cthread *p, channel *in, channel *out) {
  int value;
  while (1) {
    channel_in(in,(void *) &value, sizeof(int)); 
    value = value + 1;
    channel_out(out,(void *) &value, sizeof(int)); 
  }
}

void Nos (cthread *p, channel *out) {
  channel *a = channel_init();
  channel *b = channel_init();
  channel *c = channel_init();

  cthread *prefix = cthread_init(Prefix, 65536, 3, 0, b, a);
  cthread *delta = cthread_init(Delta, 65536, 3, a, c, out);
  cthread *succ = cthread_init(Succ, 65536, 2, c, b);

  cthread_run(prefix);
  cthread_run(delta);
  cthread_run(succ);
  cthread_join(prefix);
  cthread_join(delta);
  cthread_join(succ);
}

void Consume (cthread *p, int nLoops, channel *in) {
  int i, value;
  struct timeval t1,t2;

  fprintf(stderr, "Commstime in C ...\n");
  for (i=0; i<16; i++) {
    channel_in(in,(void *) &value,sizeof(int));
  }
  
  while(1) {
     gettimeofday(&t1,NULL);
     for (i=0; i< nLoops; i++) {
       channel_in(in,(void *) &value,sizeof(int));
     }
     gettimeofday(&t2,NULL);

     fprintf(stderr,"last value is %d\n", value);
     fprintf(stderr,"it took %f microseconds\n", calctime(&t2,&t1));
     fprintf(stderr,"it took %f ns per cs\n\n", 
             (calctime(&t2,&t1)/(8 * nLoops)) * 1000);
  }
}

void cthread_main(cthread *c, int argc, char** argv) {
  channel *chan = channel_init();
  cthread *nos = cthread_init(Nos, 65536, 1, chan);
  cthread *consume = cthread_init(Consume, 65536, 2, 1000000, chan);

  cthread_run(nos);
  cthread_run(consume);
  cthread_join(nos);
  cthread_join(consume);
}
