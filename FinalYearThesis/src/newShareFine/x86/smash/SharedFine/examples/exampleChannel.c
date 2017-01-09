#include "smash.h"
#include "channel.h"

long count = 0;
channel * chan;

void in(cthread * c) {
//    char * mess = malloc(10);
    int mess;
    while(1) {
       fprintf(stderr,"in - %d, %d\n",count++,mess);
       channel_in(chan,(void *) &mess,sizeof(int));
    }
}

void out() {
    //char * mess = "Hello";
    int mess = 1;
    while(1) {
       fprintf(stderr,"out - %d\n",count++);
       channel_out(chan,(void *) &mess,sizeof(int));
    }
}

void cthread_main(cthread * cmain, int argc, char ** argv) {
    cthread * c1, *c2; 
 
    chan = channel_init();
    c1 = cthread_init(out,40960,0);
    c2 = cthread_init(in,40960,0);

    cthread_run(c2); 
    cthread_run(c1); 
    fprintf(stderr,"This is the main user thread\n");

    cthread_join(c1);
    fprintf(stderr,"main again\n");
    cthread_join(c2);
}



