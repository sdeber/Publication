CC = gcc
INC = -I../
LIBS = -lpthread
LIBDIR = ../
SMASH = ../../smash/SharedFine/
CFLAGS = $(INC)
MUTEX = $(LIBDIR)mutex.c 
MESSAGE_QUEUE = $(LIBDIR)message_queue.c 
SEMAPHORE = $(LIBDIR)semaphore.c
CHAN = $(LIBDIR)channel.c

MUTEX_BENCH = mutex_benchmark1.out mutex_benchmark2.out mutex_benchmark3.out mutex_benchmark4.out
MESSAGE_BENCH = message_queue_benchmark.out message_queue_benchmark2.out 
SEMAPHORE_BENCH = semaphore_benchmark1.out semaphore_benchmark2.out semaphore_benchmark3.out semaphore_benchmark4.out
CHAN_BENCH =  channel_benchmark.out channel_benchmark1.out channel_benchmark2.out channel_benchmark3.out channel_benchmark4.out channel_benchmark5.out
SMASH_SRC = $(SMASH)cthread.c $(SMASH)scheduler.c $(SMASH)kernelthread.c
EXECUTABLE = $(MUTEX_BENCH) $(SEMAPHORE_BENCH) $(MESSAGE_BENCH) $(CHAN_BENCH)

all :$(MUTEX_BENCH) $(MESSAGE_BENCH) $(SEMAPHORE_BENCH) $(CHAN_BENCH)

$(MUTEX_BENCH): %.out : %.c $(SMASH_SRC) $(MUTEX)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $< $(SMASH_SRC) $(MUTEX) ../../primitives.c ../../ADTs/Fifo.c ../../ADTs/spinlock.c 

$(MESSAGE_BENCH) : %.out : %.c $(SMASH_SRC) $(MESSAGE_QUEUE)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $< $(SMASH_SRC) $(MESSAGE_QUEUE) ../../primitives.c ../../ADTs/spinlock.c ../../ADTs/generic_fifo.c

$(SEMAPHORE_BENCH) : %.out : %.c $(SMASH_SRC) $(SEMAPHORE)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $< $(SMASH_SRC) $(SEMAPHORE) ../../primitives.c ../../ADTs/Fifo.c ../../ADTs/spinlock.c 

$(CHAN_BENCH) : %.out : %.c $(SMASH_SRC) $(CHAN)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $< $(SMASH_SRC) $(CHAN) ../../primitives.c ../../ADTs/Fifo.c ../../ADTs/spinlock.c

clean :
	rm *.out
