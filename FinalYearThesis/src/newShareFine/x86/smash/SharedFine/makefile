CC = gcc
INC =   

LIBS = -lm -lsysmp

#CFLAGS = $(INC) -g  -O4  -D_REENTRANT
CFLAGS = $(INC) -g  -O0  -D_REENTRANT

OBJS = cthread.o scheduler.o channel.o kernelthread.o 

all: $(OBJS) examples benchmarks

cthread.o: cthread.c cthread.h JmpBuf.i JmpBuf.h global.h
	$(CC) $(CFLAGS) -c cthread.c 
channel.o: channel.c channel.h global.h
	$(CC) $(CFLAGS) -c channel.c
scheduler.o: scheduler.c scheduler.h global.h
	$(CC) $(CFLAGS) -c scheduler.c
kernelthread.o: kernelthread.c  kernelthread.h global.h
	$(CC) $(CFLAGS) -c kernelthread.c

clean:
	rm *.o; cd examples; make clean; cd ../benchmarks; make clean 

examples: $(OBJS)
	cd examples; make  

benchmarks: $(OBJS)
	cd benchmarks; make  
